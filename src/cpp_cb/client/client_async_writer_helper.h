// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H

#include <atomic>
#include <functional>  // for function
#include <queue>

#include <grpc_cb/impl/call_sptr.h>     // for CallSptr
#include <grpc_cb/impl/message_sptr.h>  // for MessageSptr
#include <grpc_cb/status.h>             // for Status
#include <grpc_cb/support/config.h>     // for GRPC_FINAL

namespace grpc_cb {

// Cache messages and write one by one.
class ClientAsyncWriterHelper GRPC_FINAL {
 public:
  ClientAsyncWriterHelper(const CallSptr& call_sptr,
                          std::atomic_bool& is_status_ok);
  ~ClientAsyncWriterHelper();

 public:
  using OnWritten = std::function<void()>;
  bool Write(const MessageSptr& msg_sptr, const OnWritten& on_written);
  bool IsWriting() const { return is_writing_; }
  bool WriteNext();
  bool IsWritingClosed() const { return is_writing_closed_; }
  void SetWritingClosed() { is_writing_closed_ = true; }

 private:
  const CallSptr call_sptr_;
  std::atomic_bool& is_status_ok_;
  Status status_;

  // Do not store on_written as a member variable,
  //   because on_written has a shared pointer of
  //   ClientAsyncWriterImpl/ClientAsyncReaderWriterImpl.
  struct WritingTask {
    MessageSptr msg_sptr;
    OnWritten on_written;
  };
  std::queue<WritingTask> queue_;  // cache messages to write

  bool is_writing_ = false;  // grpc only allows to write one by one
  bool is_writing_closed_ = false;
};  // class ClientAsyncWriterHelper

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H
