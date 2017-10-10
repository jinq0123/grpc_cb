// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_SERVER_SERVER_READER_FOR_BIDI_STREAMING_H
#define GRPC_CB_SERVER_SERVER_READER_FOR_BIDI_STREAMING_H

#include <memory>  // for unique_ptr<>

#include <grpc_cb_core/server/server_reader_for_bidi_streaming.h>

// XXX #include <grpc_cb/server/server_reader.h>  // for ServerReader<>
#include <grpc_cb/common/impl/config.h>  // for GRPC_OVERRIDE
#include <grpc_cb/server/server_writer.h>  // for ServerWriter<>
#include <grpc_cb/common/status_fwd.h>  // for Status

namespace grpc_cb {

// ServerReader for bidirectional streaming.
// Thread-safe.
template <class Request, class Response>
class ServerReaderForBidiStreaming
    : public grpc_cb_core::ServerReaderForBidiStreaming {
 public:
  // Default constructable.
  ServerReaderForBidiStreaming() {}
  virtual ~ServerReaderForBidiStreaming() {}

 public:
  using Writer = ServerWriter<Response>;  // NOT grpc_cb_core::ServerWriter
  // Start server reader.
  void Start(const CallSptr& call_sptr, const Writer& writer) {
    writer_uptr_.reset(new Writer(writer));
    grpc_cb_core::ServerReaderForBidiStreaming::Start(call_sptr,
                                                      writer.GetCoreWriter());
  }

 public:
  Writer& GetWriter() {
    assert(writer_uptr_);
    return *writer_uptr_;
  }

 public:
  virtual void OnMsg(const Request& msg) {}

 private:
  std::unique_ptr<Writer> writer_uptr_;
};  // class ServerReaderForBidiStreaming

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_SERVER_READER_FOR_BIDI_STREAMING_H
