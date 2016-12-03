// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H

#include <atomic>
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

class ClientAsyncReaderHelper;
class ClientAsyncWriterHelper;

class ClientAsyncReaderWriterImpl GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncReaderWriterImpl> {
 public:
  ClientAsyncReaderWriterImpl(const ChannelSptr& channel,
                              const std::string& method,
                              const CompletionQueueSptr& cq_sptr,
                              const StatusCallback& on_status);
  ~ClientAsyncReaderWriterImpl();

 public:
  bool Write(const MessageSptr& msg_sptr);
  // CloseWriting() is optional. Auto closed on dtr().
  void CloseWriting();

  // Todo: Force to close reading/writing. Cancel all reading/writing.

  using ReadHandlerSptr = ClientAsyncReadHandlerSptr;
  void ReadEach(const ReadHandlerSptr& handler_sptr);

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
  std::atomic_bool is_status_ok_{ true };

  ReadHandlerSptr read_handler_sptr_;
  StatusCallback on_status_;

  bool is_reading_ = false;  // SetReadHandler() to trigger reading.
  bool can_close_writing_ = false;  // Waiting to close?

  // Reader will be shared by CqTag.
  std::shared_ptr<ClientAsyncReaderHelper> reader_sptr_;
  std::unique_ptr<ClientAsyncWriterHelper> writer_uptr_;
};  // class ClientAsyncReaderWriterImpl<>

// Todo: BlockingGetInitMd();

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
