// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CALL_SPTR_H
#define GRPC_CB_IMPL_CALL_SPTR_H

#include <memory>

namespace grpc_cb {

class Call;
typedef std::shared_ptr<Call> CallSptr;

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CALL_SPTR_H
