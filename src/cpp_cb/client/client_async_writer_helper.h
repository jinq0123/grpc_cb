// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H

#include <functional>  // for function

#include <grpc_cb/impl/call_sptr.h>      // for CallSptr
#include <grpc_cb/impl/message_queue.h>  // for MessageQueue
#include <grpc_cb/impl/message_sptr.h>   // for MessageSptr
#include <grpc_cb/support/config.h>      // for GRPC_FINAL

namespace grpc_cb {

class Status;

// Cache messages and write one by one.
class ClientAsyncWriterHelper GRPC_FINAL {
 public:
  using OnWritten = std::function<void()>;
  ClientAsyncWriterHelper(const CallSptr& call_sptr, Status& status,
                          const OnWritten& on_written);
  ~ClientAsyncWriterHelper();

 public:
  bool Write(const MessageSptr& msg_sptr);
  bool IsWriting() const { return is_writing_; }
  bool WriteNext();
  bool IsWritingClosed() const { return is_writing_closed_; }
  void SetWritingClosed() { is_writing_closed_ = true; }

 private:
  const CallSptr call_sptr_;
  Status& status_;
  const OnWritten on_written_;
  MessageQueue msg_queue_;  // cache messages to write
  bool is_writing_ = false;  // grpc only allows to write one by one
  bool is_writing_closed_ = false;
};  // class ClientAsyncWriterHelper

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H
