// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_WRITER_IMPL_H
#define GRPC_CB_CLIENT_ASYNC_WRITER_IMPL_H

#include <functional>  // for std::function

#include <grpc_cb/impl/call_sptr.h>              // for CallSptr
#include <grpc_cb/impl/channel_sptr.h>           // for ChannelSptr
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/status.h>                      // for Status
#include <grpc_cb/support/config.h>              // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>        // for Message

namespace grpc_cb {

class ClientAsyncWriterCloseHandler;

class ClientAsyncWriterImpl GRPC_FINAL {
 public:
  ClientAsyncWriterImpl(const ChannelSptr& channel, const std::string& method,
                        const CompletionQueueSptr& cq_sptr);

  using Message = ::google::protobuf::Message;
  bool Write(const Message& request);

  using CloseHandlerSptr = std::shared_ptr<ClientAsyncWriterCloseHandler>;
  void Close(const CloseHandlerSptr& handler_sptr);

 private:
  CompletionQueueSptr cq_sptr_;
  CallSptr call_sptr_;
  Status status_;

  // XXX cache messages
};  // class ClientAsyncWriterImpl<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_ASYNC_WRITER_IMPL_H
