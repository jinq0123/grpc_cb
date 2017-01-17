// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_READER_FOR_BIDI_STREAMING_H
#define GRPC_CB_SERVER_READER_FOR_BIDI_STREAMING_H

#include <memory>  // for unique_ptr<>

#include <grpc_cb/server_reader.h>  // for ServerReader<>
#include <grpc_cb/support/config.h>  // for GRPC_OVERRIDE

namespace grpc_cb {

class Status;

template <class Response>
class ServerWriter;

// ServerReader for bidirectional streaming.
template <class Request, class Response>
class ServerReaderForBidiStreaming : public ServerReader<Request> {
 public:
  // Default constructable.
  ServerReaderForBidiStreaming() {}
  virtual ~ServerReaderForBidiStreaming() {}

 public:
  using Writer = ServerWriter<Response>;
  void SetWriter(const Writer& writer) {
    writer_uptr_.reset(new Writer(writer));
  }

 public:
  void OnMsg(const Request& msg) GRPC_OVERRIDE {}
  void OnError(const Status& status) GRPC_OVERRIDE {}
  void OnEnd() GRPC_OVERRIDE {}

 private:
  std::unique_ptr<Writer> writer_uptr_;
};  // class ServerReaderForBidiStreaming

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVER_READER_FOR_BIDI_STREAMING_H
