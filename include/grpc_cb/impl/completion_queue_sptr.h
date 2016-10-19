// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_COMPLETION_QUEUE_SPTR_H
#define GRPC_CB_COMPLETION_QUEUE_SPTR_H

#include <memory>

namespace grpc_cb {

class CompletionQueue;
typedef std::shared_ptr<CompletionQueue> CompletionQueueSptr;

}  // namespace grpc_cb

#endif  // GRPC_CB_COMPLETION_QUEUE_SPTR_H
