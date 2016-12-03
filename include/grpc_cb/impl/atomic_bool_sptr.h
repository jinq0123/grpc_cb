// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_ATOMIC_BOOL_SPTR_H
#define GRPC_CB_IMPL_ATOMIC_BOOL_SPTR_H

#include <atomic>
#include <memory>

namespace grpc_cb {
using AtomicBoolSptr = std::shared_ptr<std::atomic_bool>;
}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_ATOMIC_BOOL_SPTR_H
