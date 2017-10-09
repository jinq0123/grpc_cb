// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_SYNC_WRITER_H
#define GRPC_CB_CLIENT_SYNC_WRITER_H

#include <cassert>  // for assert()
#include <cstdint>  // for int64_t

#include <grpc_cb_core/client/channel.h>                           // for MakeSharedCall()
#include <grpc_cb_core/common/status.h>                                 // for Status
#include <grpc_cb_core/client/client_sync_writer.h>  // for grpc_cb_core::ClientSyncWriter

namespace grpc_cb {

// Copyable.
// Use template class instead of template member function
//    to ensure client input the correct request type.
template <class Request, class Response>
class ClientSyncWriter GRPC_FINAL {
 public:
  inline ClientSyncWriter(const ChannelSptr& channel, const std::string& method,
                          int64_t timeout_ms)
      : core_sptr_(new grpc_cb_core::ClientSyncWriter(
          channel, method, timeout_ms)) {}

  // Todo: SyncGetInitMd();
  bool Write(const Request& request) const {
    return core_sptr_->Write(request.SerializeAsString());
  }

  Status Close(Response* response) const {
    std::string resp_str;
    Status status = core_sptr_->Close(&resp_str);
    if (!status.ok()) return status;
    if (response->ParseFromString(resp_str))
      return status;
    return Status::InternalError("Failed to parse message "
        + response->GetTypeName());
  }

 private:
  std::shared_ptr<grpc_cb_core::ClientSyncWriter> core_sptr_;  // Easy to copy.
};  // class ClientSyncWriter<>

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_SYNC_WRITER_H
