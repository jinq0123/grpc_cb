#ifndef GRPC_CB_BLOCKING_RUN_H
#define GRPC_CB_BLOCKING_RUN_H

#include <grpc_cb/completion_queue_for_next.h>  // for CompletionQueueForNext
#include <grpc_cb/completion_queue_for_next_sptr.h>  // for CompletionQueueForNextSptr

namespace grpc_cb {

void BlockingRun(CompletionQueueForNext& cq4n);
void BlockingRun(const CompletionQueueForNextSptr& cq4n_sptr);

}  // namespace grpc_cb
#endif  // GRPC_CB_BLOCKING_RUN_H
