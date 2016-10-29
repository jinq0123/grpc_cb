// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_COMMON_DO_NEXT_COMPLETION_H
#define GRPC_CB_COMMON_DO_NEXT_COMPLETION_H

#include <grpc_cb/impl/server/server_writer_impl.h>  // for ServerWriterImpl

namespace grpc_cb {

class CompletionQueue;

// Blocking go next and complete.
// Return false if completion queue is shut down.
bool DoNextCompletion(CompletionQueue& cq);

}  // namespace grpv_cb
#endif  // GRPC_CB_COMMON_DO_NEXT_COMPLETION_H
