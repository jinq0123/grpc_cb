// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVICE_STUB_H
#define GRPC_CB_SERVICE_STUB_H

#include <atomic>  // for atomic_int64
#include <cassert>
#include <unordered_map>

#include <grpc_cb/completion_queue_for_next.h>  // for CompletionQueueForNext
#include <grpc_cb/completion_queue_for_next_sptr.h>  // for CompletionQueueForNextSptr
#include <grpc_cb/channel.h>         // for MakeSharedCall()
#include <grpc_cb/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb/impl/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb/impl/cqueue_for_next.h>   // to convert GetCq4n() to CompletionQueue
#include <grpc_cb/status_callback.h>        // for ErrorCallback
#include <grpc_cb/support/grpc_cb_api.h>    // for GRPC_CB_API

namespace grpc_cb {

// The base of generated service stubs.
// Copyable.
// Thread-safe except for set methods.
class GRPC_CB_API ServiceStub {
 public:
  explicit ServiceStub(const ChannelSptr& channel_sptr,
      const CompletionQueueForNextSptr& cq4n_sptr = nullptr);
  virtual ~ServiceStub();

 public:
  using string = std::string;

 public:
  Channel& GetChannel() const {
      assert(channel_sptr_);
      return *channel_sptr_;
  }
  // Non-null channel sptr.
  ChannelSptr GetChannelSptr() const { return channel_sptr_; }
  const ErrorCallback& GetErrorCallback() const {
    return error_callback_;
  }
  // non-thread-safe
  void SetErrorCallback(const ErrorCallback& cb) {
    error_callback_ = cb;
  }

  CompletionQueueForNextSptr GetCompletionQueue() const {
    assert(cq4n_sptr_);
    return cq4n_sptr_;
  }
  // non-thread-safe
  void SetCompletionQueue(const CompletionQueueForNextSptr& cq4n_sptr) {
    if (cq4n_sptr)
      cq4n_sptr_ = cq4n_sptr;
  }

  // ServiceStub can set timeout for all methods calls.
  int64_t GetCallTimeoutMs() const { return call_timeout_ms_; }
  void SetCallTimeoutMs(int64_t timeout_ms) {
      call_timeout_ms_ = timeout_ms;
  }

 public:
  static ErrorCallback& GetDefaultErrorCallback() {
    return default_error_callback_;
  }
  // non-thread-safe
  static void SetDefaultErrorCallback(const ErrorCallback cb) {
    default_error_callback_ = cb;
  }

 public:
  Status BlockingRequest(const string& method, const string& request,
                         string& response);

  using OnResponse = std::function<void (const string&)>;
  void AsyncRequest(const string& method, const string& request,
                    const OnResponse& on_response,
                    const ErrorCallback& on_error = GetDefaultErrorCallback());

 public:
  void BlockingRun();
  // Request the shutdown of all runs.
  void Shutdown();

 protected:
  CallSptr MakeSharedCall(const string& method) const {
    assert(cq4n_sptr_);
    return MakeSharedCall(method, *cq4n_sptr_);
  }
  CallSptr MakeSharedCall(const string& method, CompletionQueue& cq) const {
    return GetChannel().MakeSharedCall(method, cq, GetCallTimeoutMs());
  }

 private:
  const ChannelSptr channel_sptr_;
  CompletionQueueForNextSptr cq4n_sptr_;

  ErrorCallback error_callback_;
  std::atomic_int64_t call_timeout_ms_;

 private:
  static ErrorCallback default_error_callback_;
};  // class ServiceStub

}  // namespace grpc_cb

#endif  // GRPC_CB_SERVICE_STUB_H
