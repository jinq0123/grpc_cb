// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_WRITER_IMPL_H
#define GRPC_CB_CLIENT_ASYNC_WRITER_IMPL_H

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

#include <grpc_cb/impl/call_sptr.h>     // for CallSptr
#include <grpc_cb/impl/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb/impl/client/client_async_writer_impl_sptr.h>
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/impl/message_sptr.h>           // for MessageSptr
#include <grpc_cb/status.h>                      // for Status
#include <grpc_cb/support/config.h>              // for GRPC_FINAL

namespace grpc_cb {

class ClientAsyncWriterCloseHandler;
class ClientAsyncWriterHelper;
class ClientAsyncWriterCloseCqTag;

class ClientAsyncWriterImpl GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncWriterImpl> {
 public:
  ClientAsyncWriterImpl(const ChannelSptr& channel, const std::string& method,
                        const CompletionQueueSptr& cq_sptr);
  ~ClientAsyncWriterImpl();

  bool Write(const MessageSptr& request_sptr);

  using CloseHandlerSptr = std::shared_ptr<ClientAsyncWriterCloseHandler>;
  void Close(const CloseHandlerSptr& handler_sptr);
  void OnClosed(ClientAsyncWriterCloseCqTag& tag);

  // Todo: Force to close, cancel all writing.
  // Todo: get queue size

 private:
  // Write next message and close.
  void WriteNext();
  void InternalNext();
  void CloseNow();
  void CallCloseHandler();

 private:
  // The callback may lock the mutex recursively.
  mutable std::recursive_mutex mtx_;
  using Guard = std::lock_guard<std::recursive_mutex>;

  CompletionQueueSptr cq_sptr_;
  CallSptr call_sptr_;
  Status status_;
  std::atomic_bool is_status_ok_{ true };

  // Close handler hides the Response and on_closed callback.
  CloseHandlerSptr close_handler_sptr_;

  std::unique_ptr<ClientAsyncWriterHelper> writer_uptr_;
};  // class ClientAsyncWriterImpl<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_ASYNC_WRITER_IMPL_H
