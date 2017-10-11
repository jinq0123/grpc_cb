// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_SERVER_SERVER_READER_FOR_BIDI_STREAMING_H
#define GRPC_CB_SERVER_SERVER_READER_FOR_BIDI_STREAMING_H

#include <grpc_cb/server/server_reader.h>  // for ServerReader<>
#include <grpc_cb/server/server_writer.h>  // for ServerWriter<>

namespace grpc_cb {

// ServerReader for bidirectional streaming.
// Thread-safe.
template <class Request, class Response>
class ServerReaderForBidiStreaming
    : public ServerReader<Request, Response> {
 public:
  using Writer = ServerWriter<Response>;  // NOT grpc_cb_core::ServerWriter
  explicit ServerReaderForBidiStreaming(const Writer& writer)
      : writer_(writer) {}
  virtual ~ServerReaderForBidiStreaming() {}

 public:
  Writer& GetWriter() {
    return writer_;
  }

 public:
  virtual void OnMsg(const Request& msg) {}

 private:
  Writer writer_;
};  // class ServerReaderForBidiStreaming

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_SERVER_READER_FOR_BIDI_STREAMING_H
