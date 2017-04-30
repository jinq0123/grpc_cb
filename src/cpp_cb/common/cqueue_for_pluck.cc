#include <grpc_cb/impl/cqueue_for_pluck.h>

#include <cassert>

#include <grpc/grpc.h>
#include <grpc_cb/support/time.h>

namespace grpc_cb {

CQueueForPluck::CQueueForPluck()
    : CompletionQueue(grpc_completion_queue_create_for_pluck(nullptr)) {
}

CQueueForPluck::CQueueForPluck(grpc_completion_queue* take)
    : CompletionQueue(take) {
}

CQueueForPluck::~CQueueForPluck() {
}

grpc_event CQueueForPluck::PluckInternal(void* tag, gpr_timespec deadline) {
  return grpc_completion_queue_pluck(&c_cq(), tag, deadline, nullptr);
}

}  // namespace grpc_cb
