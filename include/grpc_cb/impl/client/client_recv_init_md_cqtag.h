// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_CLIENT_RECV_INIT_MD_CQTAG_H
#define GRPC_CB_IMPL_CLIENT_CLIENT_RECV_INIT_MD_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodRecvSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations

namespace grpc_cb {

// Receive init metadata.
// Used in ClientReaderWriter.
class ClientRecvInitMdCqTag : public CallCqTag {
 public:
  inline explicit ClientRecvInitMdCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {}
  inline virtual ~ClientRecvInitMdCqTag() {}
  inline bool Start() GRPC_MUST_USE_RESULT;

 private:
  CodRecvInitMd cod_recv_init_md_;
};  // class ClientRecvInitMdCqTag

bool ClientRecvInitMdCqTag::Start() {
  CallOperations ops;
  ops.RecvInitMd(cod_recv_init_md_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CLIENT_CLIENT_RECV_INIT_MD_CQTAG_H
