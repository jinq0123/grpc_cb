// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for GetMaxMsgSize()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodRecvMsg
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/protobuf_fwd.h>  // for Message

namespace grpc_cb {

// Completion queue tag to read msg blockingly.
// For ClientReader and ClientReaderWriter.
// Base class of ClientReaderAsyncReadCqTag.
class ClientReaderReadCqTag : public CallCqTag {
 public:
  explicit ClientReaderReadCqTag(const CallSptr& call_sptr)
      : CallCqTag(call_sptr) {}
  virtual ~ClientReaderReadCqTag() {}

  inline bool Start() GRPC_MUST_USE_RESULT;

  // To detect end of stream.
  inline bool HasGotMsg() const { return cod_recv_msg_.HasGotMsg(); }
  inline Status GetResultMsg(::google::protobuf::Message& message)
      GRPC_MUST_USE_RESULT {
    return cod_recv_msg_.GetResultMsg(
        message, GetCallSptr()->GetMaxMsgSize());
  }

 private:
  CodRecvMsg cod_recv_msg_;
};  // class ClientReaderReadCqTag

bool ClientReaderReadCqTag::Start() {
  CallOperations ops;
  ops.RecvMsg(cod_recv_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_READER_READ_CQTAG_H
