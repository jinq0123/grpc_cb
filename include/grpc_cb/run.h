#ifndef GRPC_CB_RUN_H
#define GRPC_CB_RUN_H

#include <grpc_cb/completion_queue_for_next.h>  // for CompletionQueueForNext
#include <grpc_cb/completion_queue_for_next_sptr.h>  // for CompletionQueueForNextSptr
#include <grpc_cb/support/grpc_cb_api.h>  // for GRPC_CB_API

namespace grpc_cb {

GRPC_CB_API void Run(CompletionQueueForNext& cq4n);
GRPC_CB_API void Run(const CompletionQueueForNextSptr& cq4n_sptr);

}  // namespace grpc_cb
#endif  // GRPC_CB_RUN_H
