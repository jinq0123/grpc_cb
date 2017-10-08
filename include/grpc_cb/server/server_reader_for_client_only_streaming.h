// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_READER_FOR_CLIENT_ONLY_STREAMING_H
#define GRPC_CB_SERVER_READER_FOR_CLIENT_ONLY_STREAMING_H

#include <memory>  // for unique_ptr<>

#include <grpc_cb/server_reader.h>  // for ServerReader<>
#include <grpc_cb/server_replier.h>  // for ReplyError()
#include <grpc_cb/common/config.h>  // for GRPC_OVERRIDE

namespace grpc_cb {

class Status;

// ServerReader for client only streaming.
// Thread-safe.
template <class Request, class Response>
class ServerReaderForClientOnlyStreaming : public ServerReader<Request> {
 public:
  // Default constructable.
  ServerReaderForClientOnlyStreaming() {}
  virtual ~ServerReaderForClientOnlyStreaming() {}

 public:
  // Set by generated code.
  using Replier = ServerReplier<Response>;
  void SetReplier(const Replier& replier) {
    replier_uptr_.reset(new Replier(replier));
  }

 public:
  void Reply(const Response& response) {
    assert(replier_uptr_);
    replier_uptr_->Reply(response);
  }
  void ReplyError(const Status& status) {
    assert(replier_uptr_);
    replier_uptr_->ReplyError(status);
  }
  Replier& GetReplier() {
    assert(replier_uptr_);
    return *replier_uptr_;
  }

 public:
  void OnMsg(const Request& msg) GRPC_OVERRIDE {}
  void OnError(const Status& status) GRPC_OVERRIDE {
    ReplyError(status);
  }
  void OnEnd() GRPC_OVERRIDE {}

 private:
  std::unique_ptr<Replier> replier_uptr_;
};  // class ServerReaderForClientOnlyStreaming

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_READER_FOR_CLIENT_ONLY_STREAMING_H
