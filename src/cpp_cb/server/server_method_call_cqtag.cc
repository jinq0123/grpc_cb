// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "server_method_call_cqtag.h"

#include <grpc_cb/service.h>  // for Service
#include <grpc_cb/impl/call.h>  // for Call
#include <grpc_cb/impl/completion_queue.h>  // for c_cq()

namespace grpc_cb {

ServerMethodCallCqTag::ServerMethodCallCqTag(grpc_server* server,
                                             Service* service,
                                             size_t method_index,
                                             void* registered_method,
                                             const CompletionQueueSptr& cq_sptr)
    : server_(server),
      service_(service),
      method_index_(method_index),
      registered_method_(registered_method),
      cq_sptr_(cq_sptr),
      call_ptr_(nullptr),
      deadline_(gpr_inf_future(GPR_CLOCK_REALTIME)),
      payload_ptr_(nullptr) {
  assert(server);
  assert(registered_method);
  assert(cq_sptr);
  assert(method_index < service->GetMethodCount());

  memset(&initial_metadata_array_, 0, sizeof(initial_metadata_array_));
  grpc_completion_queue* ccq = &cq_sptr->c_cq();
  grpc_byte_buffer **optional_payload =
      service->IsMethodClientStreaming(method_index) ?
      nullptr : &payload_ptr_;
  grpc_call_error ret = grpc_server_request_registered_call(
      server, registered_method, &call_ptr_, &deadline_,
      &initial_metadata_array_, optional_payload, ccq, ccq, this);
  assert(GRPC_CALL_OK == ret);
}

ServerMethodCallCqTag::~ServerMethodCallCqTag() {
  grpc_metadata_array_destroy(&initial_metadata_array_);
}

void ServerMethodCallCqTag::DoComplete(bool success)
{
  // TODO: check success

  // Deal payload.
  assert(service_);
  // assert(payload_ptr_);
  assert(call_ptr_);
  CallSptr call_sptr(new Call(call_ptr_));  // destroys grpc_call
  service_->CallMethod(method_index_, payload_ptr_, call_sptr);

  // Request the next method call.
  // Calls grpc_server_request_registered_call() in ctr().
  // Delete in Server::Run().
  new ServerMethodCallCqTag(server_, service_, method_index_,
                            registered_method_, cq_sptr_);
}

}  // namespace grpc_cb
