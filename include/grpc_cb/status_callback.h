// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_STATUS_CALLBACK_H
#define GRPC_CB_STATUS_CALLBACK_H

#include <functional>  // for std::function()

namespace grpc_cb {
class Status;
using StatusCallback = std::function<void (const Status& status)>;
using ErrorCallback = StatusCallback;
}  // namespace grpc_cb

#endif  // GRPC_CB_STATUS_CALLBACK_H
