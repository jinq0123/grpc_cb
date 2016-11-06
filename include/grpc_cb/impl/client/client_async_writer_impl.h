// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_WRITER_IMPL_H
#define GRPC_CB_CLIENT_ASYNC_WRITER_IMPL_H

#include <functional>  // for std::function
#include <memory>  // for enable_shared_from_this
#include <mutex>

#include <grpc_cb/impl/call_sptr.h>              // for CallSptr
#include <grpc_cb/impl/channel_sptr.h>           // for ChannelSptr
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/impl/message_queue.h>          // for MessageQueue
#include <grpc_cb/status.h>                      // for Status
#include <grpc_cb/support/config.h>              // for GRPC_FINAL

namespace grpc_cb {

class ClientAsyncWriterCloseHandler;

class ClientAsyncWriterImpl GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncWriterImpl> {
 public:
  ClientAsyncWriterImpl(const ChannelSptr& channel, const std::string& method,
                        const CompletionQueueSptr& cq_sptr);

  void Init();

  bool Write(const MessageSptr& request_sptr);

  using CloseHandlerSptr = std::shared_ptr<ClientAsyncWriterCloseHandler>;
  void Close(const CloseHandlerSptr& handler_sptr);

  // Todo: get queue size

 private:
  void Next();
  void InternalNext();

 private:
  mutable std::mutex mtx_;
  using Guard = std::lock_guard<std::mutex>;

  CompletionQueueSptr cq_sptr_;
  CallSptr call_sptr_;
  Status status_;
  // Close handler hides the Response and on_closed callback.
  CloseHandlerSptr close_handler_sptr_;
  MessageQueue msg_queue_;  // cache messages
  bool is_idle_ = true;  // Is call idle?
};  // class ClientAsyncWriterImpl<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_ASYNC_WRITER_IMPL_H
