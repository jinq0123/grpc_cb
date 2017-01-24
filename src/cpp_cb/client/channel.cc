// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/channel.h>

#include <cassert>

#include <grpc/grpc.h>  // for grpc_channel_create_call()

#include <grpc_cb/impl/call.h>
#include <grpc_cb/impl/completion_queue.h>  // for CompletionQueue

namespace grpc_cb {

Channel::Channel(const std::string& target)
    : c_channel_uptr_(grpc_insecure_channel_create(
        target.c_str(), nullptr, nullptr), grpc_channel_destroy) {
  if (!c_channel_uptr_) {
    throw("Can not create channel.");
  }
}

Channel::~Channel() {
  assert(c_channel_uptr_);
}

CallSptr Channel::MakeSharedCall(const std::string& method, CompletionQueue& cq,
                                 int64_t timeout_ms) const {
  gpr_timespec deadline =
      gpr_time_add(gpr_now(GPR_CLOCK_REALTIME),
                   gpr_time_from_micros(timeout_ms, GPR_TIMESPAN));
  return MakeSharedCall(method, cq, deadline);
}

CallSptr Channel::MakeSharedCall(const std::string& method, CompletionQueue& cq,
                                 const gpr_timespec& deadline) const {
  grpc_call* c_call = grpc_channel_create_call(
      c_channel_uptr_.get(), nullptr, GRPC_PROPAGATE_DEFAULTS, &cq.c_cq(),
      method.c_str(), nullptr, deadline, nullptr);
  return CallSptr(new Call(c_call));  // shared_ptr
}

}  // namespace grpc_cb
