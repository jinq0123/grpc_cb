// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CLIENT_IMPL_WRAP_MSG_CB_H
#define GRPC_CB_CLIENT_IMPL_WRAP_MSG_CB_H

#include <functional>

#include <grpc_cb_core/common/status.h>  // for Status
#include <grpc_cb_core/client/msg_str_cb.h>  // for MsgStrCb

namespace grpc_cb {

// Wrap message callback to string callback,
// i.e. from function<void (const HelloReply& msg)>
//        to function<Status (const string& msg_str)>
template <class Msg>
inline grpc_cb_core::MsgStrCb
WrapMsgCb(const std::function<void (const Msg&)>& cb) {
  if (!cb) return nullptr;
  return [cb](const std::string& resp_str) {
    Msg msg;
    if (msg.ParseFromString(resp_str)) {
      cb(msg);
      return Status::OK;
    }
    return Status::InternalError("Failed to parse message "
        + msg.GetTypeName());
  };  // return
}  // WrapMsgCb()

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_IMPL_WRAP_MSG_CB_H
