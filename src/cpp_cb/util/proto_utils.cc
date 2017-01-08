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

#include <google/protobuf/io/coded_stream.h>  // for SetTotalBytesLimit()
#include <google/protobuf/io/zero_copy_stream.h>  // for ZeroCopyOutputStream
#include <google/protobuf/message.h>  // for Message

#include <grpc/byte_buffer.h>  // for grpc_raw_byte_buffer_create()
#include <grpc/impl/codegen/byte_buffer_reader.h>  // for grpc_byte_buffer_reader
#include <grpc/impl/codegen/slice.h>  // for grpc_slice_buffer

#include <grpc_cb/status.h>  // for Status

#define GPR_CODEGEN_ASSERT assert

namespace grpc_cb {

namespace internal {

const int kGrpcBufferWriterMaxBufferLength = 8192;

class GrpcBufferWriter final
    : public ::google::protobuf::io::ZeroCopyOutputStream {
 public:
  explicit GrpcBufferWriter(grpc_byte_buffer** bp, int block_size)
      : block_size_(block_size), byte_count_(0), have_backup_(false) {
    *bp = grpc_raw_byte_buffer_create(NULL, 0);
    slice_buffer_ = &(*bp)->data.raw.slice_buffer;
  }

  ~GrpcBufferWriter() override {
    if (have_backup_) {
      grpc_slice_unref(backup_slice_);
    }
  }

  bool Next(void** data, int* size) override {
    if (have_backup_) {
      slice_ = backup_slice_;
      have_backup_ = false;
    } else {
      slice_ = grpc_slice_malloc(block_size_);
    }
    *data = GRPC_SLICE_START_PTR(slice_);
    // On win x64, int is only 32bit
    GPR_CODEGEN_ASSERT(GRPC_SLICE_LENGTH(slice_) <= INT_MAX);
    byte_count_ += * size = (int)GRPC_SLICE_LENGTH(slice_);
    grpc_slice_buffer_add(slice_buffer_, slice_);
    return true;
  }

  void BackUp(int count) override {
    grpc_slice_buffer_pop(slice_buffer_);
    if (count == block_size_) {
      backup_slice_ = slice_;
    } else {
      backup_slice_ = grpc_slice_split_tail(
          &slice_, GRPC_SLICE_LENGTH(slice_) - count);
      grpc_slice_buffer_add(slice_buffer_, slice_);
    }
    have_backup_ = true;
    byte_count_ -= count;
  }

  ::google::protobuf::int64 ByteCount() const override { return byte_count_; }

 private:
  const int block_size_;
  int64_t byte_count_;
  grpc_slice_buffer* slice_buffer_;
  bool have_backup_;
  grpc_slice backup_slice_;
  grpc_slice slice_;
};

class GrpcBufferReader final
    : public ::google::protobuf::io::ZeroCopyInputStream {
 public:
  explicit GrpcBufferReader(grpc_byte_buffer* buffer)
      : byte_count_(0), backup_count_(0), status_() {
    if (!grpc_byte_buffer_reader_init(&reader_,
                                                                buffer)) {
      status_ = Status::InternalError("Couldn't initialize byte buffer reader");
    }
  }
  ~GrpcBufferReader() override {
    grpc_byte_buffer_reader_destroy(&reader_);
  }

  bool Next(const void** data, int* size) override {
    if (!status_.ok()) {
      return false;
    }
    if (backup_count_ > 0) {
      *data = GRPC_SLICE_START_PTR(slice_) + GRPC_SLICE_LENGTH(slice_) -
              backup_count_;
      GPR_CODEGEN_ASSERT(backup_count_ <= INT_MAX);
      *size = (int)backup_count_;
      backup_count_ = 0;
      return true;
    }
    if (!grpc_byte_buffer_reader_next(&reader_,
                                                                &slice_)) {
      return false;
    }
    grpc_slice_unref(slice_);
    *data = GRPC_SLICE_START_PTR(slice_);
    // On win x64, int is only 32bit
    GPR_CODEGEN_ASSERT(GRPC_SLICE_LENGTH(slice_) <= INT_MAX);
    byte_count_ += * size = (int)GRPC_SLICE_LENGTH(slice_);
    return true;
  }

  Status status() const { return status_; }

  void BackUp(int count) override { backup_count_ = count; }

  bool Skip(int count) override {
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

  ::google::protobuf::int64 ByteCount() const override {
    return byte_count_ - backup_count_;
  }

 private:
  int64_t byte_count_;
  int64_t backup_count_;
  grpc_byte_buffer_reader reader_;
  grpc_slice slice_;
  Status status_;
};
}  // namespace internal

namespace Proto {

Status Serialize(const ::google::protobuf::Message& msg,
                 grpc_byte_buffer** bp) {
    int byte_size = msg.ByteSize();
    if (byte_size <= internal::kGrpcBufferWriterMaxBufferLength) {
      grpc_slice slice = grpc_slice_malloc(byte_size);
      GPR_CODEGEN_ASSERT(
          GRPC_SLICE_END_PTR(slice) ==
          msg.SerializeWithCachedSizesToArray(GRPC_SLICE_START_PTR(slice)));
      *bp = grpc_raw_byte_buffer_create(&slice, 1);
      grpc_slice_unref(slice);
      return Status::OK;
    } else {
      internal::GrpcBufferWriter writer(
          bp, internal::kGrpcBufferWriterMaxBufferLength);
      return msg.SerializeToZeroCopyStream(&writer)
                 ? Status::OK
                 : Status::InternalError("Failed to serialize message");
    }
}  // Serialize()

Status Deserialize(grpc_byte_buffer* buffer, ::google::protobuf::Message* msg,
                   int max_receive_message_size) {
    if (buffer == nullptr) {
      return Status::InternalError("No payload");
    }
    Status result = Status::OK;
    {
      internal::GrpcBufferReader reader(buffer);
      if (!reader.status().ok()) {
        return reader.status();
      }
      ::google::protobuf::io::CodedInputStream decoder(&reader);
      if (max_receive_message_size > 0) {
        decoder.SetTotalBytesLimit(max_receive_message_size,
                                   max_receive_message_size);
      }
      if (!msg->ParseFromCodedStream(&decoder)) {
        result = Status::InternalError(msg->InitializationErrorString());
      }
      if (!decoder.ConsumedEntireMessage()) {
        result = Status::InternalError("Did not read entire message");
      }
    }
    // DO NOT grpc_byte_buffer_destroy(buffer);
    return result;
}  // Deserialize()

}  // namespace Proto
}  // namespace grpc_cb
