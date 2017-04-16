#include <grpc_cb/impl/cqueue_for_next.h>

#include <grpc/grpc.h>
#include <grpc_cb/support/time.h>

namespace grpc_cb {

CQueueForNext::CQueueForNext()
    : CompletionQueue(grpc_completion_queue_create_for_next(nullptr)) {
}

CQueueForNext::CQueueForNext(grpc_completion_queue* take)
    : CompletionQueue(take) {
}

CQueueForNext::~CQueueForNext() {
}

grpc_event CQueueForNext::NextInternal(gpr_timespec deadline) {
  return grpc_completion_queue_next(&c_cq(), deadline, nullptr);
}

}  // namespace grpc_cb
