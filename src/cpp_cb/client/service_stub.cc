// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/service_stub.h>

#include <grpc_cb/impl/call.h>
#include <grpc_cb/impl/completion_queue.h>

namespace grpc_cb {

ErrorCallback ServiceStub::default_error_callback_(&ServiceStub::IgnoreError);

ServiceStub::ServiceStub(const ChannelSptr& channel_sptr) :
    channel_sptr_(channel_sptr),  // copy shared_ptr
    error_callback_(default_error_callback_),
    cq_sptr_(new CompletionQueue) {
  assert(channel_sptr);
  assert(error_callback_);
}

ServiceStub::~ServiceStub() {
  assert(cq_sptr_);
}

// Blocking run stub.
void ServiceStub::BlockingRun() {
  assert(cq_sptr_);
  CompletionQueue& cq = *cq_sptr_;
  while (true) {
    grpc_event ev = cq.Next();
    switch (ev.type) {
      case GRPC_OP_COMPLETE: {
        auto* tag = static_cast<CompletionQueueTag*>(ev.tag);
        assert(tag);
        tag->DoComplete(0 != ev.success);
        delete tag;  // DEL DeleteCompletionQueueTag(tag);  // Match NewCompletionQueueTag().
        break;
      }  // case
      case GRPC_QUEUE_SHUTDOWN:
        return;
      case GRPC_QUEUE_TIMEOUT:
        assert(false);
        break;
      default:
        assert(false);
        break;
    }  // switch
  }
}

void ServiceStub::Shutdown() {
  assert(cq_sptr_);
  cq_sptr_->Shutdown();
}

}  // namespace grpc_cb

