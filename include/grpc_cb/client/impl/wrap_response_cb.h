// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_WRAP_RESPONSE_CALLBACK_H
#define GRPC_CB_IMPL_CLIENT_WRAP_RESPONSE_CALLBACK_H

#include <functional>

#include <grpc_cb_core/common/status.h>  // for Status
#include <grpc_cb_core/client/msg_str_cb.h>  // for RespStrCb

namespace grpc_cb {

// Wrap message callback to string callback,
// i.e. from function<void (const HelloReply& response)>
//        to function<Status (const string& response)>
// Used in generated stub class.
template <class Response>
inline grpc_cb_core::RespStrCb
WrapResponseCb(const std::function<void (const Response&)>& cb) {
  if (!cb) return nullptr;
  return [cb](const std::string& resp_str) {
    Response response;
    if (response.ParseFromString(resp_str)) {
      cb(response);
      return Status::OK;
    }
    return Status::InternalError("Failed to parse response "
        + response.GetTypeName());
  };  // return
}  // WrapResponseCb()

}  // namespace grpc_cb
#endif  // GRPC_CB_IMPL_CLIENT_WRAP_RESPONSE_CALLBACK_H
