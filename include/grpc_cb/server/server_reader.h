// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_READER_H
#define GRPC_CB_SERVER_READER_H

#include <grpc_cb_core/server/server_reader.h>  // for grpc_cb_core::ServerReader

#include <grpc_cb/common/status_fwd.h>  // for Status

namespace grpc_cb {

// ServerReader is the interface of client streaming handler.
// Thread-safe.
template <class MsgType>
class ServerReader : public grpc_cb_core::ServerReader {
 public:
  ServerReader() {}
  virtual ~ServerReader() {}

 public:
  void OnMsg(const std::string& msg_str) GRPC_OVERRIDE;

 public:
  virtual void OnMsg(const MsgType& msg) {}
  virtual void OnError(const Status& status) {}
  virtual void OnEnd() {}
};  // class ServerReader

template<class MsgType>
void ServerReader<MsgType>::OnMsg(const std::string& msg_str) {
  MsgType msg;
  bool ok = msg.ParseFromString(msg_str);
  if (ok) {
    OnMsg(msg);
    return;
  }
  OnError(
      Status::InternalError("Failed to parse message " + msg.GetTypeName()));
}

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_READER_H
