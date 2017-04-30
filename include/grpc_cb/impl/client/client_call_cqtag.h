// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CALL_CQTAG_H
#define GRPC_CB_CLIENT_CALL_CQTAG_H

#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for GetMaxMsgSize()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/impl/metadata_vector.h>  // for MetadataVector
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

// Completion queue tag (CqTag) for client call for both blocking and async calls.
// ClientAsyncCallCqTag derives from it.
class ClientCallCqTag : public CallCqTag {
 public:
  explicit ClientCallCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {
    assert(call_sptr);
  }
  virtual ~ClientCallCqTag() {}

 public:
  inline bool Start(const ::google::protobuf::Message& request) GRPC_MUST_USE_RESULT;
  inline bool Start(const std::string& request) GRPC_MUST_USE_RESULT;

 public:
  Status GetResponse(::google::protobuf::Message& message) {
    // Todo: Get trailing metadata.
    if (!cod_client_recv_status_.IsStatusOk())
      return cod_client_recv_status_.GetStatus();
    return cod_recv_msg_.GetResultMsg(message, GetCallSptr()->GetMaxMsgSize());
  }
  Status GetResponseString(std::string& response) const;

 private:
  inline bool StartOps(CallOperations& ops);

 private:
  CodSendInitMd cod_send_init_md_;  // Todo: set init metadata
  CodSendMsg cod_send_msg_;
  CodRecvInitMd cod_recv_init_md_;
  CodRecvMsg cod_recv_msg_;
  CodClientRecvStatus cod_client_recv_status_;
};  // class ClientCallCqTag

bool ClientCallCqTag::Start(const ::google::protobuf::Message& request) {
  CallOperations ops;
  Status status = ops.SendMsg(request, cod_send_msg_);
  if (!status.ok()) return false;
  return StartOps(ops);
}

bool ClientCallCqTag::StartOps(CallOperations& ops) {
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitMd(cod_send_init_md_);
  ops.RecvInitMd(cod_recv_init_md_);
  ops.RecvMsg(cod_recv_msg_);
  ops.ClientSendClose();
  ops.ClientRecvStatus(cod_client_recv_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

bool ClientCallCqTag::Start(const std::string& request) {
  CallOperations ops;
  ops.SendMsg(request, cod_send_msg_);
  return StartOps(ops);
}

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CALL_CQTAG_H
