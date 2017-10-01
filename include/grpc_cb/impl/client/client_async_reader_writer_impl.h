// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H

#include <string>

#include <grpc_cb/impl/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb/impl/client/client_async_read_handler_sptr.h>  // for ClientAsyncReadHandlerSptr
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/impl/message_sptr.h>           // for MessageSptr
#include <grpc_cb/status_callback.h>             // for StatusCallback
#include <grpc_cb/support/config.h>              // for GRPC_FINAL

namespace grpc_cb {

class ClientAsyncReaderWriterImpl2;

// Thread-safe.
// Only shared in ClientAsyncReaderWriter, because we need dtr() to close writing.
class ClientAsyncReaderWriterImpl GRPC_FINAL {
 public:
  ClientAsyncReaderWriterImpl(const ChannelSptr& channel,
                              const std::string& method,
                              const CompletionQueueSptr& cq_sptr,
                              int64_t timeout_ms,
                              const StatusCallback& on_status);
  ~ClientAsyncReaderWriterImpl();

 public:
  bool Write(const MessageSptr& msg_sptr);
  // CloseWriting() is optional. Auto closed on dtr().
  void CloseWriting();

  // Todo: Force to close reading/writing. Cancel all reading/writing.

  void ReadEach(const ClientAsyncReadHandlerSptr& handler_sptr);

 private:
  // Live longer than ClientAsyncReaderWriter.
  std::shared_ptr<ClientAsyncReaderWriterImpl2> impl2_sptr_;
};  // class ClientAsyncReaderWriterImpl

// Todo: SyncGetInitMd();

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL_H
