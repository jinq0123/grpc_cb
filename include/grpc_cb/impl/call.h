// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CALL_H
#define GRPC_CB_IMPL_CALL_H

#include <atomic>
#include <cassert>
#include <mutex>

#include <grpc/grpc.h>
#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/impl/call_sptr.h>
#include <grpc_cb/support/config.h>  // for GRPC_FINAL

struct grpc_call;

namespace grpc_cb {

// Wrap of the C call object.
// Thread-safe.
// Differ from grpc++:
//  grpc_call is owned by Call instead of ClientContext/ServerContext.
class Call GRPC_FINAL {
 public:
  inline explicit Call(grpc_call* c_call);  // own

 public:
  inline bool StartBatch(const CallOperations& ops, void* tag) GRPC_MUST_USE_RESULT;
  // Todo: inline size_t GetBatchSize() const;  // c_call->batches

  inline int GetMaxMsgSize() const { return max_msg_size_; }
  inline void SetMaxMsgSize(int size) { max_msg_size_ = size; }

 public:
  static void SetDefaultMaxMsgSize(int size) { default_max_msg_size_ = size; }

 private:
  using Mutex = std::mutex;
  using Guard = std::lock_guard<Mutex>;
  Mutex mtx_;  // for grpc_call
  const std::unique_ptr<grpc_call, void (*)(grpc_call*)> c_call_uptr_;  // owned

 private:
  std::atomic_int max_msg_size_;  // Todo: necessary? Move to Channel?

 private:
  static std::atomic_int default_max_msg_size_;  // = -1;  // for all call
};

// Owns c_call.
Call::Call(grpc_call* c_call)
    : c_call_uptr_(c_call, grpc_call_unref),
      max_msg_size_(default_max_msg_size_.load()) {
  assert(c_call);
}

bool Call::StartBatch(const CallOperations& ops, void* tag) {
  // grpc_call_start_batch() is not thread-safe.
  Guard g(mtx_);
  grpc_call_error result = grpc_call_start_batch(
    c_call_uptr_.get(), ops.GetOps(), ops.GetOpsNum(), tag, nullptr);
  return GRPC_CALL_OK == result;
}

}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CALL_H
