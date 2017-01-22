// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_CLIENT_SEND_MSG_CQTAG_H
#define GRPC_CB_IMPL_CLIENT_CLIENT_SEND_MSG_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendMsg
#include <grpc_cb/impl/call_operations.h>  // for CallOperations

namespace grpc_cb {

// Base of ClientAsyncWriterSendMsgCqTag.
class ClientSendMsgCqTag : public CallCqTag {
 public:
  explicit ClientSendMsgCqTag(const CallSptr& call_sptr)
      : CallCqTag(call_sptr) {}
  virtual ~ClientSendMsgCqTag() {}

 public:
  inline bool Start(const ::google::protobuf::Message& message) GRPC_MUST_USE_RESULT;

 private:
  CodSendMsg cod_send_msg_;
};  // class ClientSendMsgCqTag

bool ClientSendMsgCqTag::Start(
    const ::google::protobuf::Message& message) {
  CallOperations ops;
  ops.SendMsg(message, cod_send_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CLIENT_CLIENT_SEND_MSG_CQTAG_H
