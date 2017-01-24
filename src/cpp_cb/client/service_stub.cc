// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/service_stub.h>

#include <grpc_cb/impl/call.h>
#include <grpc_cb/impl/completion_queue.h>
#include <grpc_cb/channel.h>  // for GetCallTimeoutMs()

#include "common/do_next_completion.h"  // for DoNextCompletion()

namespace grpc_cb {

ErrorCallback ServiceStub::default_error_callback_;  // Default empty.

ServiceStub::ServiceStub(const ChannelSptr& channel_sptr)
    : channel_sptr_(channel_sptr),  // copy shared_ptr
    cq_sptr_(new CompletionQueue),
    error_callback_(default_error_callback_),
    call_timeout_ms_(channel_sptr->GetCallTimeoutMs()) {
  assert(channel_sptr);
}

ServiceStub::~ServiceStub() {
  assert(cq_sptr_);
}

// Blocking run stub.
void ServiceStub::BlockingRun() {
  assert(cq_sptr_);
  CompletionQueue& cq = *cq_sptr_;
  while (DoNextCompletion(cq))
    ;
}

void ServiceStub::Shutdown() {
  assert(cq_sptr_);
  cq_sptr_->Shutdown();
}

}  // namespace grpc_cb

