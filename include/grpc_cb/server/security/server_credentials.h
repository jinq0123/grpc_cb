// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_SERVER_SERVER_CREDENTIALS_H
#define GRPC_CB_SERVER_SERVER_CREDENTIALS_H

#include <grpc_cb_core/server/security/server_credentials.h>

namespace grpc_cb {

using ServerCredentials = grpc_cb_core::ServerCredentials;
using InsecureServerCredentials = grpc_cb_core::InsecureServerCredentials;

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_SERVER_CREDENTIALS_H
