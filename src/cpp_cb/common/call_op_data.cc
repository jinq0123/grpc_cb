// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/call_op_data.h>

#include <grpc_cb/status.h>  // for Status
#include <grpc_cb/support/slice.h>  // for StringFromCopiedSlice()

namespace grpc_cb {

Status CodClientRecvStatus::GetStatus() const {
  return Status(status_code_, StringFromCopiedSlice(status_details_));
}

}  // namespace grpc_cb
