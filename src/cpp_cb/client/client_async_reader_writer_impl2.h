// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL2_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL2_H

#include <memory>  // for enable_shared_from_this<>
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

// Impl of impl.
// Impl1 is to make Writer copyable.
// Impl2 will live longer than the Writer.
// We need dtr() of Impl1 to close writing.
class ClientAsyncReaderWriterImpl2 GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncReaderWriterImpl2> {
 public:
  ClientAsyncReaderWriterImpl2(const ChannelSptr& channel,
                              const std::string& method,
                              const CompletionQueueSptr& cq_sptr,
                              const StatusCallback& on_status);
  ~ClientAsyncReaderWriterImpl2();

 public:
  bool Write(const MessageSptr& msg_sptr);
  // CloseWriting() is optional. Auto closed on dtr().
  void CloseWriting();

  // Todo: Force to close reading/writing. Cancel all reading/writing.

  using ReadHandlerSptr = ClientAsyncReadHandlerSptr;
  void ReadEach(const ReadHandlerSptr& handler_sptr);

 private:
  // Callback of ReaderHelper.
  void OnEndOfReading();
  // Callback of WriterHelper.
  void OnWritten();  // XXX Move into WriterHelper
  // Callback of WriterHelper.
  void OnEndOfWriting();

 private:
  // Write next message and close.
  void WriteNext();
  void CloseWritingNow();
  void SetInternalError(const std::string& sError);

 private:
  // Callbacks will lock again.
  using Mutex = std::recursive_mutex;
  mutable Mutex mtx_;
  using Guard = std::lock_guard<Mutex>;

  const CompletionQueueSptr cq_sptr_;
  const CallSptr call_sptr_;
  Status status_;

  ReadHandlerSptr read_handler_sptr_;
  StatusCallback on_status_;

  bool reading_started_ = false;  // ReadEach() to trigger reading.
  bool can_close_writing_ = false;  // Waiting to close?

  // Helper will be shared by CqTag.
  std::shared_ptr<ClientAsyncReaderHelper> reader_sptr_;
  std::shared_ptr<ClientAsyncWriterHelper> writer_sptr_;
};  // class ClientAsyncReaderWriterImpl2

// Todo: BlockingGetInitMd();

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_IMPL2_H
