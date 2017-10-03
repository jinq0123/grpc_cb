// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/service_stub.h>

#include <grpc_cb/run.h>  // for Run()
#include <grpc_cb/channel.h>  // for GetCallTimeoutMs()
#include <grpc_cb/impl/call.h>
#include <grpc_cb/impl/client/client_async_call_cqtag.h>  // for ClientAsyncCallCqTag
#include <grpc_cb/impl/client/client_call_cqtag.h>  // for ClientCallCqTag
#include <grpc_cb/impl/cqueue_for_next.h>  // for CQueueForNext
#include <grpc_cb/impl/cqueue_for_pluck.h>  // for CQueueForPluck

namespace grpc_cb {

ErrorCallback ServiceStub::default_error_callback_;  // Default empty.

ServiceStub::ServiceStub(const ChannelSptr& channel_sptr,
    const CompletionQueueForNextSptr& cq4n_sptr/* = nullptr */)
    : channel_sptr_(channel_sptr),  // copy shared_ptr
    cq4n_sptr_(cq4n_sptr),
    error_callback_(default_error_callback_),
    call_timeout_ms_(channel_sptr->GetCallTimeoutMs()) {
  assert(channel_sptr);
  // Use an internal cq if no external cq.
  if (!cq4n_sptr_)
    cq4n_sptr_.reset(new CQueueForNext);
}

ServiceStub::~ServiceStub() {
  assert(cq4n_sptr_);
}

Status ServiceStub::SyncRequest(const string& method, const string& request,
                                    string& response) {
  CQueueForPluck cq4p;
  CallSptr call_sptr(MakeSharedCall(method, cq4p));
  ClientCallCqTag tag(call_sptr);
  if (!tag.Start(request))
    return Status::InternalError("Failed to request.");
  cq4p.Pluck(&tag);
  return tag.GetResponse(response);
}

void ServiceStub::AsyncRequest(const string& method, const string& request,
                               const OnResponse& on_response,
                               const ErrorCallback& on_error) {
  CallSptr call_sptr(MakeSharedCall(method));
  using CqTag = ClientAsyncCallCqTag<std::string>;
  CqTag* tag = new CqTag(call_sptr);
  tag->SetOnResponse(on_response);
  tag->SetOnError(on_error);
  if (tag->Start(request))
    return;

  delete tag;
  if (on_error)
    on_error(Status::InternalError("Failed to async request."));
}

// Blocking run stub.
void ServiceStub::Run() {
  assert(cq4n_sptr_);
  grpc_cb::Run(cq4n_sptr_);
}

void ServiceStub::Shutdown() {
  assert(cq4n_sptr_);
  cq4n_sptr_->Shutdown();
}

}  // namespace grpc_cb

