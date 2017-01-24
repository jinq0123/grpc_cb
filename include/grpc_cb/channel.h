// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CHANNEL_H
#define GRPC_CB_CHANNEL_H

#include <atomic>  // for atomic_int64_t
#include <memory>  // for unique_ptr<>
#include <string>

#include <grpc_cb/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb/impl/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb/impl/grpc_library.h>  // for GrpcLibrary
#include <grpc_cb/support/config.h>  // for GRPC_OVERRIDE

struct grpc_channel;
struct grpc_completion_queue;

namespace grpc_cb {

class CompletionQueue;

/// Channel represents a connection to an endpoint.
/// Thread-safe.
class Channel : public GrpcLibrary,
                public std::enable_shared_from_this<Channel> {
 public:
  explicit Channel(const std::string& target);
  virtual ~Channel() GRPC_OVERRIDE;

 public:
  void SetCallTimeoutMs(int64_t timeout_ms) { call_timeout_ms_ = timeout_ms; }
  int64_t GetCallTimeoutMs() const { return call_timeout_ms_; }

 public:
  CallSptr MakeSharedCall(const std::string& method, CompletionQueue& cq) const;
  CallSptr MakeSharedCall(const std::string& method, CompletionQueue& cq,
                          int64_t timeout_ms) const;

 private:
  CallSptr MakeSharedCall(const std::string& method, CompletionQueue& cq,
                          const gpr_timespec& deadline) const;

 private:
  const std::unique_ptr<grpc_channel, void (*)(grpc_channel*)>
      c_channel_uptr_;  // owned
  std::atomic_int64_t call_timeout_ms_{ INT64_MAX };
};

}  // namespace grpc_cb

#endif  // GRPC_CB_CHANNEL_H
