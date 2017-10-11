// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_SERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H
#define GRPC_CB_SERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H

#include <grpc_cb/server/server_reader.h>  // for ServerReader<>
#include <grpc_cb/server/server_replier.h>  // for ReplyError()
#include <grpc_cb/common/status_fwd.h>  // for Status

namespace grpc_cb {

// ServerReader for client only streaming.
// Thread-safe.
template <class Request, class Response>
class ServerReaderForClientSideStreaming
    : public ServerReader<Request, Response> {
 public:
  using Replier = ServerReplier<Response>;  // NOT grpc_cb_core::ServerReplier
  explicit ServerReaderForClientSideStreaming(const Replier& replier)
      : replier_(replier) {}
  virtual ~ServerReaderForClientSideStreaming() {}

 public:
  void Reply(const Response& response) {
    replier_.Reply(response);
  }
  void ReplyError(const Status& status) {
    replier_.ReplyError(status);
  }
  Replier& GetReplier() {
    return replier_;
  }

 public:
  virtual void OnMsg(const Request& msg) {}

 private:
  Replier replier_;
};  // class ServerReaderForClientSideStreaming

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H
