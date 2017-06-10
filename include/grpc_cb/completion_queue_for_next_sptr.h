// Shared pointer to complete queue for next.
#ifndef GRPC_CB_COMPLETION_QUEUE_FOR_NEXT_SPTR_H
#define GRPC_CB_COMPLETION_QUEUE_FOR_NEXT_SPTR_H

#include <memory>

namespace grpc_cb {

class CQueueForNext;  // = CompletionQueueForNext
using CompletionQueueForNextSptr = std::shared_ptr<CQueueForNext>;

}  // namespace grpc_cb
#endif  // GRPC_CB_COMPLETION_QUEUE_FOR_NEXT_SPTR_H
