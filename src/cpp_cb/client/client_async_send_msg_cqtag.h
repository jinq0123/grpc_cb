// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_SEND_MSG_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_SEND_MSG_CQTAG_H

#include <functional>  // for function

#include <grpc_cb/impl/client/client_send_msg_cqtag.h>  // for ClientSendMsgCqTag
#include <grpc_cb/support/config.h>                     // for GRPC_FINAL

#include "client_async_writer_helper_sptr.h"  // for ClientAsyncWriterHelperSptr

namespace grpc_cb {

class ClientAsyncSendMsgCqTag GRPC_FINAL : public ClientSendMsgCqTag {
 public:
  explicit ClientAsyncSendMsgCqTag(
      const ClientAsyncWriterHelperSptr writer_sptr);

  void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  const ClientAsyncWriterHelperSptr writer_sptr_;
};  // class ClientAsyncSendMsgCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_SEND_MSG_CQTAG_H
