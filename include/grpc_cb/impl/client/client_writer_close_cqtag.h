// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_WRITER_CLOSE_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_WRITER_CLOSE_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodRecvMsg
#include <grpc_cb/impl/call_operations.h>  // for CallOperations

namespace grpc_cb {

// Base of ClientAsyncWriterCloseCqTag.
class ClientWriterCloseCqTag : public CallCqTag {
 public:
  explicit ClientWriterCloseCqTag(const CallSptr& call_sptr)
      : CallCqTag(call_sptr) {}
  virtual ~ClientWriterCloseCqTag() {}

  inline bool Start() GRPC_MUST_USE_RESULT;
  inline bool IsStatusOk() const {
    return cod_client_recv_status_.IsStatusOk();
  }
  inline Status GetStatus() const {
    return cod_client_recv_status_.GetStatus();
  }
  inline Status GetResponse(::google::protobuf::Message& response) GRPC_MUST_USE_RESULT;

 private:
  // XXX recv init md
  CodRecvMsg cod_recv_msg_;
  CodClientRecvStatus cod_client_recv_status_;
};  // class ClientWriterCloseCqTag

bool ClientWriterCloseCqTag::Start() {
  CallOperations ops;
  ops.ClientSendClose();  // XXX separate send and recv
  ops.RecvMsg(cod_recv_msg_);
  ops.ClientRecvStatus(cod_client_recv_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

Status ClientWriterCloseCqTag::GetResponse(
    ::google::protobuf::Message& response) {
  return cod_recv_msg_.GetResultMsg(
        response, GetCallSptr()->GetMaxMsgSize());
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_WRITER_CLOSE_CQTAG_H
