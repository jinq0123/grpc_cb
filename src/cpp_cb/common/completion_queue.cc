#include <grpc_cb/impl/completion_queue.h>

#include <cassert>

#include <grpc/grpc.h>

namespace grpc_cb {

CompletionQueue::CompletionQueue(grpc_completion_queue* take)
    : c_cq_uptr_(take, grpc_completion_queue_destroy) {
  assert(c_cq_uptr_);
}

CompletionQueue::~CompletionQueue() {
  assert(c_cq_uptr_);
}

void CompletionQueue::Shutdown() {
  assert(c_cq_uptr_);
  grpc_completion_queue_shutdown(c_cq_uptr_.get());
}

}  // namespace grpc_cb
