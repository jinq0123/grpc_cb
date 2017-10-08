// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_READER_H
#define GRPC_CB_CLIENT_ASYNC_READER_H

#include <cstdint>  // for int64_t
#include <functional>  // for std::function
#include <string>

#include <grpc_cb_core/client/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb_core/client/client_async_reader.h>  // for grpc_cb_core::ClientAsyncReader
//#include <grpc_cb/impl/client/client_async_read_handler.h>  // for ClientAsyncReadHandler
//#include <grpc_cb/impl/client/client_async_reader_impl.h>  // for ClientAsyncReaderImpl
#include <grpc_cb_core/common/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/client/status_cb.h>  // for StatusCb
#include <grpc_cb/common/protobuf_fwd.h>        // for Message

namespace grpc_cb {

// Copyable. Thread-safe.
template <class Request, class Response>
class ClientAsyncReader GRPC_FINAL {
 public:
  ClientAsyncReader(const grpc_cb_core::ChannelSptr& channel,
                    const std::string& method,
                    const Request& request,
                    const grpc_cb_core::CompletionQueueSptr& cq_sptr,
                    int64_t timeout_ms)
      : core_sptr_(new grpc_cb_core::ClientAsyncReader(channel, method,
                   request.SerializeAsString(), cq_sptr, timeout_ms)) {}

 public:
  using MsgCb = std::function<void(const Response&)>;
  void ReadEach(const MsgCb& on_msg,
      const StatusCb& status_cb = StatusCb()) const {
    class ReadHandler : public ClientAsyncReadHandler {
     public:
      explicit ReadHandler(const MsgCb& on_msg) : on_msg_(on_msg) {}
      Message& GetMsg() GRPC_OVERRIDE { return msg_; }
      void HandleMsg() GRPC_OVERRIDE { if (on_msg_) on_msg_(msg_); }
     private:
      MsgCb on_msg_;
      Response msg_;
    };

    auto handler_sptr = std::make_shared<ReadHandler>(on_msg);
    impl_sptr_->SetReadHandler(handler_sptr);
    impl_sptr_->SetOnStatus(status_cb);
    impl_sptr_->Start();
  }

 private:
  const std::shared_ptr<grpc_cb_core::ClientAsyncReader> core_sptr_;
};  // class ClientAsyncReader<>

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_ASYNC_READER_H
