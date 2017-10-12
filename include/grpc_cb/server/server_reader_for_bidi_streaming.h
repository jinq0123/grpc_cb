// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_SERVER_SERVER_READER_FOR_BIDI_STREAMING_H
#define GRPC_CB_SERVER_SERVER_READER_FOR_BIDI_STREAMING_H

#include <memory>  // for unique_ptr<>

#include <grpc_cb_core/common/support/config.h>  // for GRPC_OVERRIDE

#include <grpc_cb/common/status_fwd.h>     // for Status
#include <grpc_cb/server/server_reader.h>  // for ServerReader<>
#include <grpc_cb/server/server_writer.h>  // for ServerWriter<>

namespace grpc_cb {

// ServerReader for bidirectional streaming.
// Thread-safe.
template <class Request, class Response>
class ServerReaderForBidiStreaming
    : public ServerReader<Request, Response> {
 public:
  // Default constructable.
  ServerReaderForBidiStreaming() {}
  virtual ~ServerReaderForBidiStreaming() {}

 public:
  // Default close writing on error.
  // User can override it to do other things.
  void OnError(const Status& status) GRPC_OVERRIDE {
    assert(writer_uptr_);  // Must after Start().
    writer_uptr_->AsyncClose(status);
  }

 public:
  using Writer = ServerWriter<Response>;  // NOT grpc_cb_core::ServerWriter

  // Start server reader.
  void Start(const CallSptr& call_sptr, const Writer& writer) {
    writer_uptr_.reset(new Writer(writer));
    ServerReader<Request, Response>::Start(call_sptr);
  }

 public:
  Writer& GetWriter() {
    assert(writer_uptr_);  // Must after Start().
    return *writer_uptr_;
  }

 private:
  std::unique_ptr<Writer> writer_uptr_;
};  // class ServerReaderForBidiStreaming

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_SERVER_READER_FOR_BIDI_STREAMING_H
