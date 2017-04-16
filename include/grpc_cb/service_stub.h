// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_STUB_H
#define GRPC_CB_SERVICE_STUB_H

#include <atomic>  // for atomic_int64
#include <cassert>
#include <unordered_map>

#include <grpc_cb/channel.h>         // for MakeSharedCall()
#include <grpc_cb/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb/impl/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/impl/completion_queue_tag.h>   // for CompletionQueueTag
#include <grpc_cb/status_callback.h>             // for StatusCallback
#include <grpc_cb/support/config.h>              // for GRPC_OVERRIDE
#include <grpc_cb/support/grpc_cb_api.h>  // for GRPC_CB_API

namespace grpc_cb {

// The base of generated service stubs.
// Copyable.
// Thread-safe except for SetErrorCallback() and SetDefaultErrorCallback().
class GRPC_CB_API ServiceStub {
 public:
  explicit ServiceStub(const ChannelSptr& channel_sptr);
  virtual ~ServiceStub();

 public:
  inline Channel& GetChannel() const {
      assert(channel_sptr_);
      return *channel_sptr_;
  }
  inline ChannelSptr GetChannelSptr() const { return channel_sptr_; }
  inline const ErrorCallback& GetErrorCallback() const {
    return error_callback_;
  }
  // non-thread-safe
  inline void SetErrorCallback(const ErrorCallback& cb) {
    error_callback_ = cb;
  }
  inline CompletionQueue& GetCq() const {
    assert(cq_sptr_);
    return *cq_sptr_;
  }
  inline CompletionQueueSptr GetCqSptr() const { return cq_sptr_; }
  inline int64_t GetCallTimeoutMs() const { return call_timeout_ms_; }
  inline void SetCallTimeoutMs(int64_t timeout_ms) {
      call_timeout_ms_ = timeout_ms;
  }
  CallSptr MakeSharedCall(const std::string& method) const {
    return MakeSharedCall(method, GetCq());
  }
  CallSptr MakeSharedCall(const std::string& method, CompletionQueue& cq) const {
    return GetChannel().MakeSharedCall(method, cq, GetCallTimeoutMs());
  }

 public:
  static inline ErrorCallback& GetDefaultErrorCallback() {
    return default_error_callback_;
  }
  // non-thread-safe
  static inline void SetDefaultErrorCallback(const ErrorCallback cb) {
    default_error_callback_ = cb;
  }

 public:
  void BlockingRun();
  // Request the shutdown of all runs.
  void Shutdown();

 private:
  const ChannelSptr channel_sptr_;
  const CompletionQueueSptr cq_sptr_;

  ErrorCallback error_callback_;
  std::atomic_int64_t call_timeout_ms_;

 private:
  static ErrorCallback default_error_callback_;
};  // class ServiceStub

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_STUB_H
