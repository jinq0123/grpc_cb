// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_SYNC_READER_WRITER_H
#define GRPC_CB_CLIENT_SYNC_READER_WRITER_H

#include <cstdint>  // for int64_t
#include <memory>  // for shared_ptr
#include <string>

#include <grpc_cb_core/client/client_sync_reader_writer.h>  // for grpc_cb_core::ClientSyncReaderWriter

namespace grpc_cb {

// Copyable.
template <class Request, class Response>
class ClientSyncReaderWriter GRPC_FINAL {
 public:
  ClientSyncReaderWriter(const ChannelSptr& channel, const std::string& method,
                         int64_t timeout_ms)
      : core_sptr_(new grpc_cb_core::ClientSyncReaderWriter(
          channel, method, timeout_ms)) {
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

  // Same as ClientSyncReader::ReadOne()
  bool ReadOne(Response* response) const {
    assert(response);
    std::string resp_str;
    if (!core_sptr_->ReadOne(&resp_str)) return false;
    if (response->ParseFromString(resp_str)) return true;
    core_sptr_->SetErrorStatus(Status::InternalError("Failed to parse message "
        + response.GetTypeName()));
    return false;
  }

  Status RecvStatus() const {
    return core_sptr_->RecvStatus();
  }

 private:
  const std::shared_ptr<grpc_cb_core::ClientSyncReaderWriter> core_sptr_;
};  // class ClientSyncReaderWriter<>

// Todo: SyncGetInitMd();

// Todo: same as ClientReader?

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_SYNC_READER_WRITER_H
