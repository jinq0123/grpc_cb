// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_READER_H
#define GRPC_CB_SERVER_READER_H

namespace grpc_cb {

class Status;

// ServerReader is the interface of client streaming handler.
// Thread-safe.
template <class MsgType>
class ServerReader {
 public:
  ServerReader() {}
  virtual ~ServerReader() {}

 public:
  virtual void OnMsg(const MsgType& msg) {}
  virtual void OnError(const Status& status) {}
  virtual void OnEnd() {}
};  // class ServerReader

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_READER_H
