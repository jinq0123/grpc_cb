// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CLIENT_IMPL_WRAP_CLOSE_CB_H
#define GRPC_CB_CLIENT_IMPL_WRAP_CLOSE_CB_H

#include <functional>

#include <grpc_cb_core/common/status.h>  // for Status
#include <grpc_cb_core/client/close_cb.h>  // for CloseCb

namespace grpc_cb {

using Status = grpc_cb_core::Status;

// Wrap close callback to string callback,
// i.e. from function<void (const Status&, const Response&)>
//        to function<void (const Status&, const std::string&)>
// Used in ClientAsyncWriter class.
template <class Response>
inline grpc_cb_core::CloseCb
WrapCloseCb(const std::function<void (const Status&, const Response&)>& cb) {
  if (!cb) return nullptr;
  return [cb](const Status& status, const std::string& resp_str) {
    Response response;
    if (!status.ok())
    {
      cb(status, response);  // keep original error
      return;
    }
    if (response.ParseFromString(resp_str)) {
      cb(status, response);
      return;
    }
    // Parsing failed.
    cb(Status::InternalError("Failed to parse response "
        + response.GetTypeName()), response);
  };  // return
}  // WrapCloseCb()

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_IMPL_WRAP_CLOSE_CB_H
