// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_READER_H
#define GRPC_CB_CLIENT_ASYNC_READER_H

#include <cstdint>  // for int64_t
#include <functional>  // for std::function
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb_core/client/client_async_reader.h>  // for grpc_cb_core::ClientAsyncReader

#include <grpc_cb/client/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/client/status_cb.h>  // for StatusCb
#include <grpc_cb/common/impl/config.h>     // for GRPC_FINAL

namespace grpc_cb {

// Copyable. Thread-safe.
template <class Request, class Response>
class ClientAsyncReader GRPC_FINAL {
 public:
  ClientAsyncReader(const grpc_cb_core::ChannelSptr& channel,
                    const std::string& method,
                    const Request& request,
                    const CompletionQueueSptr& cq_sptr,
                    int64_t timeout_ms)
      : core_sptr_(new grpc_cb_core::ClientAsyncReader(channel, method,
                   request.SerializeAsString(), cq_sptr, timeout_ms)) {}

 public:
  using MsgCb = std::function<void(const Response&)>;
  void ReadEach(const MsgCb& msg_cb,
      const StatusCb& status_cb = StatusCb()) const {
    grpc_cb_core::MsgStrCb msg_str_cb =
      [msg_cb, status_cb](const std::string& sResponse) {
        Response response;
        bool ok = response.ParseFromString(sResponse);
        if (ok) return Status::OK;
        return Status::InternalError("Failed to parse message "
            + response.GetTypeName());
      };
    core_sptr_->ReadEach(msg_str_cb, status_cb)
  }

 private:
  const std::shared_ptr<grpc_cb_core::ClientAsyncReader> core_sptr_;
};  // class ClientAsyncReader<>

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_ASYNC_READER_H
