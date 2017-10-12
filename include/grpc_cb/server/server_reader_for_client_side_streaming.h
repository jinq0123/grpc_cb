// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_SERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H
#define GRPC_CB_SERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H

#include <memory>  // for unique_ptr<>

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
  // Default constructable.
  ServerReaderForClientSideStreaming() {}
  virtual ~ServerReaderForClientSideStreaming() {}

 public:
  // Default reply error on error.
  // User can override it to do other things.
  void OnError(const Status& status) GRPC_OVERRIDE {
    ReplyError(status);
  }

 public:
  using Replier = ServerReplier<Response>;  // NOT grpc_cb_core::ServerReplier

  // Start server reader.
  void Start(const CallSptr& call_sptr, const Replier& replier) {
    replier_uptr_.reset(new Replier(replier));
    ServerReader<Request, Response>::Start(call_sptr);
  }

 public:
  void Reply(const Response& response) {
    assert(replier_uptr_);  // Must after Start().
    replier_uptr_->Reply(response);
  }
  void ReplyError(const Status& status) {
    assert(replier_uptr_);  // Must after Start().
    replier_uptr_->ReplyError(status);
  }

  Replier& GetReplier() {
    assert(replier_uptr_);  // Must after Start().
    return *replier_uptr_;
  }

 private:
  std::unique_ptr<Replier> replier_uptr_;
};  // class ServerReaderForClientSideStreaming

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H
