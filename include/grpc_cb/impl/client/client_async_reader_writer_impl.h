// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H

#include <memory>
#include <mutex>
#include <string>

#include <grpc_cb/impl/call_sptr.h>     // for CallSptr
#include <grpc_cb/impl/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb/impl/client/client_async_read_handler_sptr.h>  // for ClientAsyncReadHandlerSptr
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/impl/message_sptr.h>           // for MessageSptr
#include <grpc_cb/status.h>                      // for Status
#include <grpc_cb/status_callback.h>             // for StatusCallback
#include <grpc_cb/support/config.h>              // for GRPC_FINAL

namespace grpc_cb {

class ClientAsyncWriterHelper;

// Todo: make it thread-safe
class ClientAsyncReaderWriterImpl GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncReaderWriterImpl> {
 public:
  ClientAsyncReaderWriterImpl(const ChannelSptr& channel,
                              const std::string& method,
                              const CompletionQueueSptr& cq_sptr);
  ~ClientAsyncReaderWriterImpl();

 public:
  bool Write(const MessageSptr& msg_sptr);
  // CloseWriting() is optional. Auto closed on dtr().
  void CloseWriting();

  using ReadHandlerSptr = ClientAsyncReadHandlerSptr;
  void SetReadHandler(const ReadHandlerSptr& handler_sptr);
  void SetOnEnd(const StatusCallback& on_status) {
    on_status_ = on_status;
  }

 private:
  // Write next message and close.
  void WriteNext();
  void InternalNext();
  void CloseWritingNow();

 private:
  mutable std::mutex mtx_;
  using Guard = std::lock_guard<std::mutex>;

  CompletionQueueSptr cq_sptr_;
  CallSptr call_sptr_;
  Status status_;

  ReadHandlerSptr read_handler_sptr_;
  StatusCallback on_status_;

  bool is_reading_ = false;  // SetReadHandler() to trigger reading.
  bool can_close_writing_ = false;  // Waiting to close?
  bool writing_closed_ = false;  // Is close sent?

  std::unique_ptr<ClientAsyncWriterHelper> writer_uptr_;
};  // class ClientAsyncReaderWriterImpl<>

// Todo: BlockingGetInitMd();

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
