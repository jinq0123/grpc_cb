// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H

#include <functional>  // for function

// #include <grpc_cb/impl/call_sptr.h>                    // for CallSptr
#include <grpc_cb/support/config.h>   // for GRPC_FINAL
#include <grpc_cb/impl/message_queue.h>          // for MessageQueue
#include <grpc_cb/impl/message_sptr.h>           // for MessageSptr
#include <grpc_cb/impl/call_sptr.h>              // for CallSptr

namespace grpc_cb {

class Status;

// Cache messages and write one by one.
class ClientAsyncWriterHelper GRPC_FINAL {
 public:
  using OnWritten = std::function<void()>;
  explicit ClientAsyncWriterHelper(const CallSptr& call_sptr, Status& status,
                                   const OnWritten& on_written);
  ~ClientAsyncWriterHelper();

 public:
  bool Write(const MessageSptr& msg_sptr);
  bool IsWriting() const { return is_writing_; }
  bool WriteNext();

 private:
  const CallSptr call_sptr_;
  Status& status_;
  const OnWritten on_written_;
  MessageQueue msg_queue_;  // cache messages to write
  bool is_writing_ = false;  // grpc only allows to write one by one
};  // class ClientAsyncWriterHelper

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H
