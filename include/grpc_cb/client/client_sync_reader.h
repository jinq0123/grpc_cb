// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_SYNC_READER_H
#define GRPC_CB_CLIENT_SYNC_READER_H

#include <cassert>  // for assert()
#include <cstdint>  // for int64_t

#include <grpc_cb_core/client/client_sync_reader.h>  // for grpc_cb_core::ClientSyncReader

#include <grpc_cb/common/status_fwd.h>    // for Status

namespace grpc_cb {

// Copyable. Client sync reader.
template <class Request, class Response>
class ClientSyncReader GRPC_FINAL {
 public:
  // Todo: Also need to template request?
  ClientSyncReader(const ChannelSptr& channel, const std::string& method,
                   const Request& request, int64_t timeout_ms)
      : core_sptr_(new ClientSyncReader(channel, method,
                   request.SerializeAsString(), timeout_ms)) {
    assert(channel);
  }

 public:
  // Return false if error or end of stream.
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
  std::shared_ptr<grpc_cb_core::ClientSyncReader> core_sptr_;
};  // class ClientSyncReader<>

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_SYNC_READER_H
