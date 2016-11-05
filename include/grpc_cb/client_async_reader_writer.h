// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_H

#include <grpc_cb/impl/client/client_async_reader_writer_impl.h>  // for ClientAsyncReaderWriterImpl<>

namespace grpc_cb {

// Copyable.
template <class Request, class Response>
class ClientAsyncReaderWriter GRPC_FINAL {
 public:
  ClientAsyncReaderWriter(const ChannelSptr& channel,
                            const std::string& method,
                            const CompletionQueueSptr& cq_sptr)
      // Todo: same as ClientReader?
      : impl_sptr_(new Impl(channel, method, cq_sptr)) {
    assert(cq_sptr);
    assert(channel);
  }

 public:
  bool Write(const Request& request) const {
    return impl_sptr_->Write(request);
  }

  // Optional. Writing is auto closed in dtr().
  // Redundant calls are ignored.
  void CloseWriting() {
    impl_sptr_->CloseWriting();
  }

  using ReadCallback = std::function<void(const Response&)>;
  void ReadEach(
      const ReadCallback& on_read,
      const StatusCallback& on_status = StatusCallback()) const {
    impl_sptr_->ReadEach(on_read, on_status);
  }

 private:
  using Impl = ClientAsyncReaderWriterImpl<Request, Response>;
  std::shared_ptr<Impl> impl_sptr_;
};  // class ClientAsyncReaderWriter<>

// Todo: BlockingGetInitMd();

// Todo: same as ClientReader?

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_WRITER_H
