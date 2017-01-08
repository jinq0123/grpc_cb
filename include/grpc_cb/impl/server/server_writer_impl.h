// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_WRITER_IMPL_H
#define GRPC_CB_SERVER_SERVER_WRITER_IMPL_H

#include <limits>  // for numeric_limits<>
#include <memory>  // for enable_shared_from_this<>
#include <mutex>

#include <grpc_cb/impl/call_sptr.h>              // for CallSptr
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/impl/message_queue.h>          // for MessageQueue
#include <grpc_cb/support/config.h>              // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>        // for Message

namespace grpc_cb {

class Status;

// Thread-safe.
class ServerWriterImpl GRPC_FINAL
    : public std::enable_shared_from_this<ServerWriterImpl> {
 public:
  ServerWriterImpl(const CallSptr& call_sptr,
                   const CompletionQueueSptr& cq_sptr);
  ~ServerWriterImpl();  // blocking

 public:
  // Write() will block if the high queue size reached.
  bool Write(const ::google::protobuf::Message& response);
  bool BlockingWrite(const ::google::protobuf::Message& response);
  bool AsyncWrite(const ::google::protobuf::Message& response);

  size_t GetQueueSize() const {
    Guard g(mtx_);
    return queue_.size();
  }
  size_t GetHighQueueSize() const {
    Guard g(mtx_);
    return high_queue_size_;
  }
  void SetHighQueueSize(size_t high_queue_size) {
    Guard g(mtx_);
    high_queue_size_ = high_queue_size;
  }

  // Close() is optional. Dtr() will auto BlockingClose().
  // Redundant Close() will be ignored.
  void BlockingClose(const Status& status);
  void AsyncClose(const Status& status);
  bool IsClosed() const {
    Guard g(mtx_);
    return closed_;
  }

 public:
  void TryToWriteNext();  // for ServerWriterWriteCqTag::DoComplete()

 private:
  void SendStatus();  // to close
  bool SendMsg(const ::google::protobuf::Message& msg);

 private:
  CallSptr call_sptr_;
  CompletionQueueSptr cq_sptr_;
  bool closed_ = false;
  bool send_init_md_ = true;  // to send initial metadata once
  bool is_sending_ = false;  // grpc must send one by one

  size_t high_queue_size_ = std::numeric_limits<size_t>::max();
  MessageQueue queue_;
  std::unique_ptr<Status> close_status_uptr_;

  mutable std::mutex mtx_;
  using Guard = std::lock_guard<std::mutex>;
};  // class ServerWriterImpl

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_WRITER_IMPL_H
