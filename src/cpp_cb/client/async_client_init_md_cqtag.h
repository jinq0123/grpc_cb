// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_CLIENT_INIT_MD_CQTAG_H
#define GRPC_CB_CLIENT_ASYNC_CLIENT_INIT_MD_CQTAG_H

#include <functional>  // for std::function<>

#include <grpc_cb/impl/client/client_init_md_cqtag.h>  // for ClientInitMdCqTag
#include <grpc_cb/support/config.h>                    // for GRPC_FINAL

namespace grpc_cb {

// Send and recv init metadata, and do next step on completion.
// Used in ClientAsyncWriter and ClientAsyncReaderWriter.
class AsyncClientInitMdCqTag GRPC_FINAL : public ClientInitMdCqTag {
 public:
  AsyncClientInitMdCqTag(const CallSptr& call_sptr)
      : ClientInitMdCqTag(call_sptr) {}

  using CompleteCallback = std::function<void()>;
  void SetCompleteCb(const CompleteCallback& on_complete) {
    on_complete_ = on_complete;
  }

 public:
  void DoComplete(bool success) override {
    assert(success);  // Todo ?
    if (on_complete_) on_complete_();
  }

 private:
  CompleteCallback on_complete_;
};  // class AsyncClientInitMdCqTag

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_ASYNC_CLIENT_INIT_MD_CQTAG_H
