// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_READER_WRITER_H
#define GRPC_CB_CLIENT_ASYNC_READER_WRITER_H

#include <cassert>

#include <grpc_cb/impl/client/client_async_read_handler.h>  // for ClientAsyncReadHandler
#include <grpc_cb/impl/client/client_async_reader_writer_impl.h>  // for ClientAsyncReaderWriterImpl<>
#include <grpc_cb/status_callback.h>  // for StatusCallback
#include <grpc_cb/support/config.h>   // for GRPC_FINAL

namespace grpc_cb {

// Copyable.
template <class Request, class Response>
class ClientAsyncReaderWriter GRPC_FINAL {
 public:
  ClientAsyncReaderWriter(const ChannelSptr& channel, const std::string& method,
                          const CompletionQueueSptr& cq_sptr,
                          const StatusCallback& on_status = StatusCallback())
      : impl_sptr_(new Impl(channel, method, cq_sptr, on_status)) {
    assert(cq_sptr);
    assert(channel);
  }

 public:
  bool Write(const Request& request) const {
    auto msg_sptr = std::make_shared<Request>(request);
    return impl_sptr_->Write(msg_sptr);
  }

  // Optional. Writing is auto closed in dtr().
  // Redundant calls are ignored.
  void CloseWriting() {
    impl_sptr_->CloseWriting();
  }

  using OnRead = std::function<void(const Response&)>;
  void ReadEach(const OnRead& on_read) {

    class ReadHandler : public ClientAsyncReadHandler {
     public:
      explicit ReadHandler(const OnRead& on_read) : on_read_(on_read) {}
      Message& GetMsg() GRPC_OVERRIDE { return msg_; }
      void HandleMsg() GRPC_OVERRIDE { if (on_read_) on_read_(msg_); }
     private:
      OnRead on_read_;
      Response msg_;
    };

    auto handler_sptr = std::make_shared<ReadHandler>(on_read);
    impl_sptr_->ReadEach(handler_sptr);
  }

 private:
  using Impl = ClientAsyncReaderWriterImpl;
  const std::shared_ptr<Impl> impl_sptr_;
};  // class ClientAsyncReaderWriter<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_ASYNC_READER_WRITER_H
