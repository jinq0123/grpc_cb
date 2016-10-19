// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_SEND_CLOSE_CQTAG_H
#define GRPC_CB_CLIENT_CLIENT_SEND_CLOSE_CQTAG_H

#include <grpc/support/port_platform.h>    // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call.h>             // for StartBatch()
#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/support/config.h>        // for GRPC_FINAL

namespace grpc_cb {

class ClientSendCloseCqTag GRPC_FINAL : public CallCqTag {
 public:
  inline explicit ClientSendCloseCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {}
  inline bool Start() GRPC_MUST_USE_RESULT;
};  // class ClientInitMdCqTag

bool ClientSendCloseCqTag::Start() {
  CallOperations ops;
  ops.ClientSendClose();
  return GetCallSptr()->StartBatch(ops, this);
}

};  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_SEND_CLOSE_CQTAG_H
