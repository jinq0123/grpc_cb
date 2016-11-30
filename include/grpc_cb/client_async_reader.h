// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_READER_H
#define GRPC_CB_CLIENT_ASYNC_READER_H

#include <functional>  // for std::function
#include <string>

#include <grpc_cb/impl/channel_sptr.h>                      // for ChannelSptr
#include <grpc_cb/impl/client/client_async_read_handler.h>  // for ClientAsyncReadHandler
#include <grpc_cb/impl/client/client_async_reader_impl.h>  // for ClientAsyncReaderImpl
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/status_callback.h>             // for StatusCallback
#include <grpc_cb/support/protobuf_fwd.h>        // for Message

namespace grpc_cb {

// Copyable. Thread-safe.
template <class Response>
class ClientAsyncReader GRPC_FINAL {
 public:
  ClientAsyncReader(const ChannelSptr& channel, const std::string& method,
                    const ::google::protobuf::Message& request,
                    const CompletionQueueSptr& cq_sptr)
      : impl_sptr_(
            new ClientAsyncReaderImpl(channel, method, request, cq_sptr)) {}

 public:
  using OnMsg = std::function<void(const Response&)>;
  void ReadEach(const OnMsg& on_msg,
      const StatusCallback& on_status = StatusCallback()) const {
    class ReadHandler : public ClientAsyncReadHandler {
     public:
      explicit ReadHandler(const OnMsg& on_msg) : on_msg_(on_msg) {}
      Message& GetMsg() GRPC_OVERRIDE { return msg_; }
     private:
      OnMsg on_msg_;
      Response msg_;
    };

    auto handler_sptr = std::make_shared<ReadHandler>(on_msg);
    impl_sptr_->SetReadHandler(handler_sptr);
    impl_sptr_->SetOnStatus(on_status);
    impl_sptr_->Start();
  }

 private:
  std::shared_ptr<ClientAsyncReaderImpl> impl_sptr_;
};  // class ClientAsyncReader<>

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_ASYNC_READER_H
