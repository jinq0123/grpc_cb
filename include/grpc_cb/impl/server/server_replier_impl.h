// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_REPLIER_IMPL_H
#define GRPC_CB_SERVER_REPLIER_IMPL_H

#include <atomic>  // for atomic_bool
#include <memory>

#include <grpc_cb/impl/call_sptr.h>                   // for CallSptr
#include <grpc_cb/impl/server/server_replier_cqtag.h>  // for ServerReplierCqTag

namespace grpc_cb {

class Status;

// ServerReplierImpl can reply any type of response.
// User should not use ServerReplierImpl directly, use ServerReplier<> instead.
// Safe to delete before completion.
// Thread-safe.
class ServerReplierImpl GRPC_FINAL {
 public:
  explicit ServerReplierImpl(const CallSptr& call_sptr)
      : call_sptr_(call_sptr) {
    assert(call_sptr);
    assert(!replied_);
  };

 public:
  // XXX Reply(string)
  template <class ResponseType>
  void Reply(const ResponseType& response) {
    if (replied_) return;
    replied_ = true;
    auto* tag = new ServerReplierCqTag(call_sptr_, send_init_md_);  // delete in Run()
    if (!tag->StartReply(response)) delete tag;
  }

  void ReplyError(const Status& status) {
    if (replied_) return;
    replied_ = true;
    auto* tag = new ServerReplierCqTag(call_sptr_, send_init_md_);  // delete in Run()
    if (!tag->StartReplyError(status)) delete tag;
  }

private:
  const CallSptr call_sptr_;
  std::atomic_bool send_init_md_{ true };  // need to send initial metadata
  std::atomic_bool replied_{ false };
};  // class ServerReplierImpl

// Todo: SendInitMetadata and SetTrailingMetadata

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_REPLIER_IMPL_H
