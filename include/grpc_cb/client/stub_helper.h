// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_STUB_HELPER_H
#define GRPC_CB_CLIENT_STUB_HELPER_H

#include <grpc_cb_core/common/status.h>  // for Status

#include <grpc_cb/client/service_stub.h>  // for ServiceStub
#include <grpc_cb/client/status_cb.h>  // for ErrorCb
#include <grpc_cb/client/impl/wrap_msg_cb.h>  // for WrapMsgCb()
#include <grpc_cb/common/impl/protobuf_fwd.h>  // for Message

namespace grpc_cb {

// Helper to request stub.
class StubHelper {
public:
    StubHelper(ServiceStub& stub) : stub_(stub) {}

public:
    using Message = ::google::protobuf::Message;

    template <class Response>
    inline Status SyncRequest(const std::string& method,
        const Message& request, Response* response);

    template <class Response>
    inline void AsyncRequest(const std::string& method,
        const Message& request,
        const std::function<void (const Response&)>& cb,
        const ErrorCb& ecb);

private:
    ServiceStub& stub_;
};  // StubHelper

template <class Response>
Status StubHelper::SyncRequest(const std::string& method,
    const Message& request, Response* response) {
  std::string resp_str;
  ::grpc_cb::Status status = stub_.SyncRequest(method,
      request.SerializeAsString(), resp_str);
  if (!status.ok() || !response)
    return status;
  if (response->ParseFromString(resp_str))
    return status;
  return status.InternalError("Failed to parse response "
      + response->GetTypeName());
}

template <class Response>
void StubHelper::AsyncRequest(const std::string& method,
    const Message& request,
    const std::function<void (const Response&)>& cb,
    const ErrorCb& ecb) {
  stub_.AsyncRequest(method, request.SerializeAsString(),
      WrapMsgCb(cb), ecb);
}

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_STUB_HELPER_H
