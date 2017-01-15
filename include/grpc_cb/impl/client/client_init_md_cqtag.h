// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_CLIENT_INIT_MD_CQTAG_H
#define GRPC_CB_IMPL_CLIENT_CLIENT_INIT_MD_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodSendInitMd
#include <grpc_cb/impl/call_operations.h>  // for CallOperations

namespace grpc_cb {

// Send and recv init metadata.
// Used in ClientWriter and ClientReaderWriter.
class ClientInitMdCqTag : public CallCqTag {
 public:
  inline explicit ClientInitMdCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {}
  inline virtual ~ClientInitMdCqTag() {}
  inline bool Start() GRPC_MUST_USE_RESULT;

 private:
  CodSendInitMd cod_send_init_md_;
  CodRecvInitMd cod_recv_init_md_;
};  // class ClientInitMdCqTag

bool ClientInitMdCqTag::Start() {
  CallOperations ops;
  // Todo: Fill send_init_md_array_ -> FillMetadataVector()
  ops.SendInitMd(cod_send_init_md_);
  ops.RecvInitMd(cod_recv_init_md_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CLIENT_CLIENT_INIT_MD_CQTAG_H
