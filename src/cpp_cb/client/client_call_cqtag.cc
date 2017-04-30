// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/impl/client/client_call_cqtag.h>

namespace grpc_cb {

Status ClientCallCqTag::GetResponse(std::string& response) const {
    // Todo: Get trailing metadata.
    if (!cod_client_recv_status_.IsStatusOk())
      return cod_client_recv_status_.GetStatus();
    return cod_recv_msg_.GetResultString(response);
}

}  // namespace grpc_cb
