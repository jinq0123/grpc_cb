// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_GENERAL_CALL_CQTAG_H
#define GRPC_CB_IMPL_GENERAL_CALL_CQTAG_H

#include <cassert>  // for assert()
#include <functional>  // for function<>

#include <grpc_cb/impl/call_cqtag.h>  // for CallCqTag
#include <grpc_cb/support/config.h>   // for GRPC_OVERRIDE

namespace grpc_cb {

// Call completion queue tag with OnComplete callback.
class GeneralCallCqTag : public CallCqTag {
 public:
  explicit GeneralCallCqTag(const CallSptr& call_sptr) : CallCqTag(call_sptr) {
    assert(call_sptr);
  }

 public:
  using OnComplete = std::function<void (bool success)>;
  void SetOnComplete(const OnComplete& on_complete) {
    on_complete_ = on_complete;
  }

  void DoComplete(bool success) GRPC_OVERRIDE {
    if (on_complete_)
      on_complete_(success);
  }

 private:
  OnComplete on_complete_;
};  // class GeneralCallCqTag

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_GENERAL_CALL_CQTAG_H
