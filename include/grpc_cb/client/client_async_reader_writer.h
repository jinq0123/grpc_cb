// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_READER_WRITER_H
#define GRPC_CB_CLIENT_ASYNC_READER_WRITER_H

#include <cassert>
#include <cstdint>  // for int64_t

#include <grpc_cb_core/client/client_async_reader_writer.h>  // for grpc_cb_core::ClientAsyncReaderWriter

#include <grpc_cb/client/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb/client/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/client/status_cb.h>  // for StatusCb
#include <grpc_cb/common/impl/config.h>     // for GRPC_FINAL

namespace grpc_cb {

// Copyable. Thread-safe.
template <class Request, class Response>
class ClientAsyncReaderWriter GRPC_FINAL {
 public:
  // Todo: Move status_cb to Set()
  ClientAsyncReaderWriter(const ChannelSptr& channel, const std::string& method,
                          const CompletionQueueSptr& cq_sptr,
                          int64_t timeout_ms,
                          const StatusCb& status_cb = nullptr)
      : core_sptr_(new grpc_cb_core::ClientAsyncReaderWriter(
          channel, method, cq_sptr, timeout_ms, status_cb)) {
    assert(cq_sptr);
    assert(channel);
  }

 public:
  bool Write(const Request& request) const {
    return core_sptr_->Write(request.SerializeAsString());
  }

  // Optional. Writing is auto closed in dtr().
  // Redundant calls are ignored.
  void CloseWriting() {
    core_sptr_->CloseWriting();
  }

  using ReadCb = MsgCbTmpl<Response>;
  void ReadEach(const ReadCb& read_cb) {
    grpc_cb_core::MsgStrCb msg_str_cb = WrapMsgCb(read_cb);
    core_sptr_->ReadEach(msg_str_cb);
  }

 private:
  const std::shared_ptr<grpc_cb_core::ClientAsyncReaderWriter> core_sptr_;
};  // class ClientAsyncReaderWriter<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_ASYNC_READER_WRITER_H
