/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <grpc_cb/impl/proto_utils.h>

#include <climits>

#include <google/protobuf/io/coded_stream.h>  // for SetTotalBytesLimit()
#include <google/protobuf/io/zero_copy_stream.h>  // for ZeroCopyOutputStream
#include <google/protobuf/message.h>  // for Message

#include <grpc/grpc.h>
#include <grpc/byte_buffer.h>
#include <grpc/byte_buffer_reader.h>
#include <grpc/support/log.h>
#include <grpc/support/slice.h>
#include <grpc/support/slice_buffer.h>
#include <grpc/support/port_platform.h>

#include <grpc_cb/support/config.h>
#include <grpc_cb/status.h>  // for Status

// #include "src/core/profiling/timers.h"  // for GPR_TIMER_SCOPE()

const int kMaxBufferLength = 8192;

class GrpcBufferWriter GRPC_FINAL
    : public ::google::protobuf::io::ZeroCopyOutputStream {
 public:
  explicit GrpcBufferWriter(grpc_byte_buffer** bp,
                            int block_size = kMaxBufferLength)
      : block_size_(block_size), byte_count_(0), have_backup_(false) {
    *bp = grpc_raw_byte_buffer_create(NULL, 0);
    slice_buffer_ = &(*bp)->data.raw.slice_buffer;
  }

  ~GrpcBufferWriter() GRPC_OVERRIDE {
    if (have_backup_) {
      gpr_slice_unref(backup_slice_);
    }
  }

  bool Next(void** data, int* size) GRPC_OVERRIDE {
    if (have_backup_) {
      slice_ = backup_slice_;
      have_backup_ = false;
    } else {
      slice_ = gpr_slice_malloc(block_size_);
    }
    *data = GPR_SLICE_START_PTR(slice_);
    // On win x64, int is only 32bit
    GPR_ASSERT(GPR_SLICE_LENGTH(slice_) <= INT_MAX);
    byte_count_ += * size = (int)GPR_SLICE_LENGTH(slice_);
    gpr_slice_buffer_add(slice_buffer_, slice_);
    return true;
  }

  void BackUp(int count) GRPC_OVERRIDE {
    gpr_slice_buffer_pop(slice_buffer_);
    if (count == block_size_) {
      backup_slice_ = slice_;
    } else {
      backup_slice_ =
          gpr_slice_split_tail(&slice_, GPR_SLICE_LENGTH(slice_) - count);
      gpr_slice_buffer_add(slice_buffer_, slice_);
    }
    have_backup_ = true;
    byte_count_ -= count;
  }

  ::google::protobuf::int64 ByteCount() const GRPC_OVERRIDE { return byte_count_; }

 private:
  const int block_size_;
  int64_t byte_count_;
  gpr_slice_buffer* slice_buffer_;
  bool have_backup_;
  gpr_slice backup_slice_;
  gpr_slice slice_;
};

class GrpcBufferReader GRPC_FINAL
    : public ::google::protobuf::io::ZeroCopyInputStream {
 public:
  explicit GrpcBufferReader(grpc_byte_buffer* buffer)
      : byte_count_(0), backup_count_(0) {
    grpc_byte_buffer_reader_init(&reader_, buffer);
  }
  ~GrpcBufferReader() GRPC_OVERRIDE {
    grpc_byte_buffer_reader_destroy(&reader_);
  }

  bool Next(const void** data, int* size) GRPC_OVERRIDE {
    if (backup_count_ > 0) {
      *data = GPR_SLICE_START_PTR(slice_) + GPR_SLICE_LENGTH(slice_) -
              backup_count_;
      GPR_ASSERT(backup_count_ <= INT_MAX);
      *size = (int)backup_count_;
      backup_count_ = 0;
      return true;
    }
    if (!grpc_byte_buffer_reader_next(&reader_, &slice_)) {
      return false;
    }
    gpr_slice_unref(slice_);
    *data = GPR_SLICE_START_PTR(slice_);
    // On win x64, int is only 32bit
    GPR_ASSERT(GPR_SLICE_LENGTH(slice_) <= INT_MAX);
    byte_count_ += * size = (int)GPR_SLICE_LENGTH(slice_);
    return true;
  }

  void BackUp(int count) GRPC_OVERRIDE { backup_count_ = count; }

  bool Skip(int count) GRPC_OVERRIDE {
    const void* data;
    int size;
    while (Next(&data, &size)) {
      if (size >= count) {
        BackUp(size - count);
        return true;
      }
      // size < count;
      count -= size;
    }
    // error or we have too large count;
    return false;
  }

  ::google::protobuf::int64 ByteCount() const GRPC_OVERRIDE {
    return byte_count_ - backup_count_;
  }

 private:
  int64_t byte_count_;
  int64_t backup_count_;
  grpc_byte_buffer_reader reader_;
  gpr_slice slice_;
};

namespace grpc_cb {

Status SerializeProto(const ::google::protobuf::Message& msg,
                      grpc_byte_buffer** bp) {
  // GPR_TIMER_SCOPE("SerializeProto", 0);
  int byte_size = msg.ByteSize();
  if (byte_size <= kMaxBufferLength) {
    gpr_slice slice = gpr_slice_malloc(byte_size);
    GPR_ASSERT(GPR_SLICE_END_PTR(slice) ==
               msg.SerializeWithCachedSizesToArray(GPR_SLICE_START_PTR(slice)));
    *bp = grpc_raw_byte_buffer_create(&slice, 1);
    gpr_slice_unref(slice);
    return Status::OK;
  } else {
    GrpcBufferWriter writer(bp);
    return msg.SerializeToZeroCopyStream(&writer)
               ? Status::OK
               : Status::InternalError("Failed to serialize message");
  }
}

// Todo: Convert grpc_byte_buffer to string, which is needed by lua.

// max_msg_size <= 0 means no limit.
Status DeserializeProto(grpc_byte_buffer* buffer,
                        ::google::protobuf::Message* msg,
                        int max_msg_size) {
  // GPR_TIMER_SCOPE("DeserializeProto", 0);
  if (!buffer) {
    return Status::InternalError("No payload");
  }
  GrpcBufferReader reader(buffer);
  ::google::protobuf::io::CodedInputStream decoder(&reader);
  if (max_msg_size > 0) {
    decoder.SetTotalBytesLimit(max_msg_size, max_msg_size);
  }
  if (!msg->ParseFromCodedStream(&decoder)) {
    return Status::InternalError(msg->InitializationErrorString());
  }
  if (!decoder.ConsumedEntireMessage()) {
    return Status::InternalError("Did not read entire message");
  }
  return Status::OK;
}

}  // namespace grpc_cb
