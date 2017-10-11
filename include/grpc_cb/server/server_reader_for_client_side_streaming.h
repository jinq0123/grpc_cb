// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_SERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H
#define GRPC_CB_SERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H

#include <memory>  // for unique_ptr<>

#include <grpc_cb_core/server/server_reader_for_client_side_streaming.h>

// XXX #include <grpc_cb/server/server_reader.h>  // for ServerReader<>
#include <grpc_cb/server/server_replier.h>  // for ReplyError()
#include <grpc_cb/common/impl/config.h>  // for GRPC_OVERRIDE
#include <grpc_cb/common/status_fwd.h>  // for Status

namespace grpc_cb {

// ServerReader for client only streaming.
// Thread-safe.
template <class Request, class Response>
class ServerReaderForClientSideStreaming
    : public grpc_cb_core::ServerReaderForClientSideStreaming {
 public:
  // Default constructable.
  ServerReaderForClientSideStreaming() {}
  virtual ~ServerReaderForClientSideStreaming() {}

 public:
  using Replier = ServerReplier<Response>;  // NOT grpc_cb_core::ServerReplier
  // Start server reader.
  void Start(const CallSptr& call_sptr, const Replier& replier) {
    replier_uptr_.reset(new Replier(replier));
    grpc_cb_core::ServerReaderForClientSideStreaming::Start(
        call_sptr, replier.GetCoreReplier());
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
  void OnMsgStr(const std::string& msg_str) GRPC_OVERRIDE;

 public:
  virtual void OnMsg(const Request& msg) {}

 private:
  std::unique_ptr<Replier> replier_uptr_;
};  // class ServerReaderForClientSideStreaming

template <class Request, class Response>
void ServerReaderForClientSideStreaming<Request, Response>::OnMsgStr(
    const std::string& msg_str) {
  Request msg;
  bool ok = msg.ParseFromString(msg_str);
  if (ok) {
    OnMsg(msg);
    return;
  }
  OnError(
      Status::InternalError("Failed to parse message " + msg.GetTypeName()));
}

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_SERVER_READER_FOR_CLIENT_SIDE_STREAMING_H
