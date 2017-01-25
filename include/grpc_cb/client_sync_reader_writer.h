// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_SYNC_READER_WRITER_H
#define GRPC_CB_CLIENT_SYNC_READER_WRITER_H

#include <cstdint>  // for int64_t
#include <memory>  // for shared_ptr
#include <string>

#include <grpc_cb/impl/client/client_sync_reader_writer_impl.h>  // for ClientSyncReaderWriterImpl<>

namespace grpc_cb {

// Copyable.
template <class Request, class Response>
class ClientSyncReaderWriter GRPC_FINAL {
 public:
  ClientSyncReaderWriter(const ChannelSptr& channel, const std::string& method, int64_t timeout_ms)
      : impl_sptr_(new Impl(channel, method, timeout_ms)) {
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

  bool ReadOne(Response* response) const {
    return impl_sptr_->ReadOne(response);
  }

  Status RecvStatus() const {
    return impl_sptr_->RecvStatus();
  }

 private:
  using Impl = ClientSyncReaderWriterImpl<Request, Response>;
  const std::shared_ptr<Impl> impl_sptr_;
};  // class ClientSyncReaderWriter<>

// Todo: BlockingGetInitMd();

// Todo: same as ClientReader?

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_SYNC_READER_WRITER_H
