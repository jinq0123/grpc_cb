// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H

#include <memory>  // for enable_shared_from_this<>

#include <grpc_cb/impl/atomic_bool_sptr.h>  // for AtomicBoolSptr
#include <grpc_cb/impl/call_sptr.h>         // for CallSptr
#include <grpc_cb/impl/message_queue.h>     // for MessageQueue
#include <grpc_cb/impl/message_sptr.h>      // for MessageSptr
#include <grpc_cb/status.h>                 // for Status
#include <grpc_cb/support/config.h>         // for GRPC_FINAL

namespace grpc_cb {

// Cache messages and write one by one.
// Differ from ClientAsyncReaderHelper:
//  Reader is ended by the peer, while writer is ended by the caller.
//  When the caller is destructed, WriterHelper must be informed that
//    there are no more writing.
//  And WriterHelper must live longer than the caller.
class ClientAsyncWriterHelper GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncWriterHelper> {
 public:
   // XXX add on_end?
  ClientAsyncWriterHelper(const CallSptr& call_sptr,
                          const AtomicBoolSptr& status_ok_sptr);
  ~ClientAsyncWriterHelper();

 public:
  bool Write(const MessageSptr& msg_sptr);
  bool IsWriting() const { return is_writing_; }
  bool WriteNext();
  bool IsWritingClosed() const { return is_writing_closed_; }
  void SetWritingClosed() { is_writing_closed_ = true; }

 private:
  const CallSptr call_sptr_;
  const AtomicBoolSptr status_ok_sptr_;
  Status status_;

  MessageQueue msg_queue_;  // cache messages to write

  bool is_writing_ = false;  // grpc only allows to write one by one
  bool is_writing_closed_ = false;
};  // class ClientAsyncWriterHelper

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_WRITER_HELPER_H
