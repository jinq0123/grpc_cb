// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/call_op_data.h>

#include <grpc_cb/status.h>  // for Status

namespace grpc_cb {

Status CodClientRecvStatus::GetStatus() const {
  return Status(status_code_,
                status_details_ ? std::string(status_details_) : "");
}

}  // namespace grpc_cb
