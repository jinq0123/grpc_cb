// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_SERVER_SERVER_READER_H
#define GRPC_CB_SERVER_SERVER_READER_H

#include <grpc_cb_core/server/server_reader.h>  // for grpc_cb_core::ServerReader

#include <grpc_cb/common/impl/config.h>  // for GRPC_OVERRIDE
#include <grpc_cb/common/status_fwd.h>   // for Status

namespace grpc_cb {

// ServerReader is the interface of client streaming handler,
//  for both client-side streaming and bi-directional streaming.
// Thread-safe.
// Must be shared pointer because it is enabled shared by this.
template <class Request, class Response>
class ServerReader : public grpc_cb_core::ServerReader {
 public:
  ServerReader() {}
  virtual ~ServerReader() {}

 public:
  Status OnMsgStr(const std::string& msg_str) GRPC_OVERRIDE;

 public:
  virtual void OnMsg(const Request& msg) {}
};  // class ServerReader

template <class Request, class Response>
Status ServerReader<Request, Response>::OnMsgStr(const std::string& msg_str) {
  Request msg;
  if (!msg.ParseFromString(msg_str))
    return Status::InternalError("Failed to parse message " + msg.GetTypeName());
  OnMsg(msg);  // Calls subclass's implemention.
  return Status::OK;
}  // OnMsg()

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_SERVER_READER_H
