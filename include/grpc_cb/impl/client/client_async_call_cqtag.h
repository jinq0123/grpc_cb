// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H
#define GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H

#include <grpc_cb/impl/client/client_call_cqtag.h>  // for ClientCallCqTag
#include <grpc_cb/status_callback.h>                // for ErrorCallback

namespace grpc_cb {

// Completion queue tag (CqTag) for client async call.
// Derived from ClientCallCqTag, adding cb, ecb.
template <class ResponseType>
class ClientAsyncCallCqTag : public ClientCallCqTag {
 public:
  using ResponseCallback = std::function<void (const ResponseType&)>;

 public:
  ClientAsyncCallCqTag(const CallSptr call_sptr, const ResponseCallback& cb,
                      const ErrorCallback& ecb)
     : ClientCallCqTag(call_sptr), cb_(cb), ecb_(ecb) {}
  virtual ~ClientAsyncCallCqTag() {}

 public:
  void DoComplete(bool success) GRPC_OVERRIDE {
    assert(success);
    ResponseType resp;
    Status status = GetResponse(resp);
    if (status.ok()) {
      if (cb_) cb_(resp);
      return;
    }
    if (ecb_) ecb_(status);
  };  // Todo: What is the use of 'success'?

 private:
  ResponseCallback cb_;
  ErrorCallback ecb_;
};  // class ClientAsyncCallCqTag

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_ASYNC_CALL_CQTAG_H
