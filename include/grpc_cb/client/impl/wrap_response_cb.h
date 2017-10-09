// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_WRAP_RESPONSE_CALLBACK_H
#define GRPC_CB_IMPL_CLIENT_WRAP_RESPONSE_CALLBACK_H

#include <functional>

#include <grpc_cb_core/common/status.h>  // for Status
#include <grpc_cb/client/status_cb.h>  // for ErrorCb

namespace grpc_cb {

// Wrap message callback to string callback,
// i.e. from function<void (const HelloReply& response)>
//        to function<void (const string& response)>
// Used in generated stub class.
template <class Msg>
inline
std::function<void (const std::string&)>
WrapResponseCb(
    const std::function<void (const Msg&)>& cb,
    const ErrorCb& ecb) {
  if (!cb) return nullptr;
  return [cb, ecb](const std::string& resp_str) {
    Msg msg;
    if (msg.ParseFromString(resp_str)) {
      cb(msg);
      return;
    }
    if (ecb)
      ecb(Status::InternalError("Failed to parse response."));
  };  // return
}  // WrapResponseCb()

}  // namespace grpc_cb
#endif  // GRPC_CB_IMPL_CLIENT_WRAP_RESPONSE_CALLBACK_H
