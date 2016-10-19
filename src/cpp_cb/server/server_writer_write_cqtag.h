// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_WRITER_WRITE_CQTAG_H
#define GRPC_CB_SERVER_SERVER_WRITER_WRITE_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendMsg
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

class ServerWriterWriteCqTag GRPC_FINAL : public CallCqTag {
 public:
  inline explicit ServerWriterWriteCqTag(const CallSptr& call_sptr)
      : CallCqTag(call_sptr) {}
  inline bool Start(const ::google::protobuf::Message& message,
    bool send_init_md) GRPC_MUST_USE_RESULT;

 private:
  CodSendInitMd cod_send_init_md_;
  CodSendMsg cod_send_msg_;
};  // class ServerWriterWriteCqTag

bool ServerWriterWriteCqTag::Start(
    const ::google::protobuf::Message& message, bool send_init_md) {
  CallOperations ops;
  if (send_init_md) {
    // Todo: set init_md
    ops.SendInitMd(cod_send_init_md_);
  }
  ops.SendMsg(message, cod_send_msg_);
  return GetCallSptr()->StartBatch(ops, this);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_WRITER_WRITE_CQTAG_H
