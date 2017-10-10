// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_REPLIER_H
#define GRPC_CB_SERVER_REPLIER_H

#include <memory>

#include <grpc_cb_core/server/server_replier.h>  // for grpc_cb_core::ServerReplier

#include <grpc_cb/common/impl/call_sptr.h>       // for CallSptr
#include <grpc_cb/common/status_fwd.h>  // for Status

namespace grpc_cb {

// ServerReplier is for unary rpc call and client only streaming rpc.
// ServerWriter is for server streaming rpc.
// ServerReader is provided by user for client streaming rpc.

// Copyable. Thread-safe.
// Safe to delete before completion.
// Only accept the 1st reply and ignore other replies.
template <class Response>
class ServerReplier {
 public:
  explicit ServerReplier(const CallSptr& call_sptr)
      : core_sptr_(new grpc_cb_core::ServerReplier(call_sptr)) {
    assert(call_sptr);
  };
  virtual ~ServerReplier() {};

 public:
  grpc_cb_core::ServerReplier& GetCoreReplier() const {
    return *core_sptr_;
  }

 public:
  void Reply(const Response& response) const {
    core_sptr_->ReplyStr(response.SerializeAsString());
  }

  void ReplyError(const Status& status) const {
    core_sptr_->ReplyError(status);
  }

private:
  const std::shared_ptr<grpc_cb_core::ServerReplier> core_sptr_;  // copyable
};  // class ServerReplier

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_REPLIER_H
