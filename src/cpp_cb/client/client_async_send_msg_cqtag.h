// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_SEND_MSG_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_SEND_MSG_CQTAG_H

#include <functional>  // for function

#include <grpc_cb/impl/client/client_send_msg_cqtag.h>  // for ClientSendMsgCqTag
#include <grpc_cb/support/config.h>                     // for GRPC_FINAL

namespace grpc_cb {

class ClientAsyncSendMsgCqTag GRPC_FINAL : public ClientSendMsgCqTag {
 public:
  using OnComplete = std::function<void()>;
  ClientAsyncSendMsgCqTag(const CallSptr& call_sptr,
                                   const OnComplete& on_complete);

  void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  const OnComplete on_complete_;
};  // class ClientAsyncSendMsgCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_SEND_MSG_CQTAG_H
