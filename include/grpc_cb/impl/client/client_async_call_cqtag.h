// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H
#define GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H

#include <grpc_cb/impl/client/client_call_cqtag.h>  // for ClientCallCqTag
#include <grpc_cb/status_callback.h>                // for ErrorCallback
#include <grpc_cb/support/config.h>                 // for GRPC_FINAL

namespace grpc_cb {

// Completion queue tag (CqTag) for client async call.
// Derived from ClientCallCqTag, adding on_response, on_error.
template <class ResponseType>
class ClientAsyncCallCqTag GRPC_FINAL : public ClientCallCqTag {
 public:
  explicit ClientAsyncCallCqTag(const CallSptr call_sptr)
     : ClientCallCqTag(call_sptr) {}

 public:
  using OnResponse = std::function<void (const ResponseType&)>;
  void SetOnResponse(const OnResponse& on_response) {
    on_response_ = on_response;
  }
  void SetOnError(const ErrorCallback& on_error) {
    on_error_ = on_error;
  }

 public:
  void DoComplete(bool success) GRPC_OVERRIDE {
    if (!success) {
      if (on_error_)
        on_error_(Status::InternalError("ClientAsyncCallCqTag failed."));
      return;
    }

    ResponseType resp;
    Status status = GetResponse(resp);
    if (status.ok()) {
      if (on_response_)
        on_response_(resp);
      return;
    }
    if (on_error_)
      on_error_(status);
  };

 private:
  OnResponse on_response_;
  ErrorCallback on_error_;
};  // class ClientAsyncCallCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H
