// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_WRITER_H
#define GRPC_CB_SERVER_WRITER_H

#include <grpc_cb/common/impl/call_sptr.h>  // for CallSptr
#include <grpc_cb_core/server/server_writer.h>  // for grpc_cb_core::ServerWriter

namespace grpc_cb {

// XXX Derive it from grpc_cb_core::ServerWriter...

// Copyable. Thread-safe.
template <class Response>
class ServerWriter GRPC_FINAL {
 public:
  explicit ServerWriter(const CallSptr& call_sptr)
      : core_sptr_(new grpc_cb_core::ServerWriter(call_sptr)) {
    assert(call_sptr);
  }

 public:
  grpc_cb_core::ServerWriter& GetCoreWriter() const {
    return *core_sptr_;
  }

 public:
  bool Write(const Response& response) const {
    return core_sptr_->Write(response.SerializeAsString());
  }
  bool SyncWrite(const Response& response) const {
    return core_sptr_->SyncWrite(response.SerializeAsString());
  }
  void AsyncWrite(const Response& response) const {
    core_sptr_->AsyncWrite(response.SerializeAsString());
  }

  size_t GetQueueSize() const {
    return core_sptr_->GetQueueSize();
  }
  size_t GetHighQueueSize() const {
    return core_sptr_->GetHighQueueSize();
  }
  void SetHighQueueSize(size_t high_queue_size) {
    core_sptr_->SetHighQueueSize(high_queue_size);
  }

  // Close() is optional. Dtr() will auto SyncClose().
  // Redundant close will be ignored.
  void SyncClose(const Status& status) const {
    core_sptr_->SyncClose(status);
  }
  void AsyncClose(const Status& status) const {
    core_sptr_->AsyncClose(status);
  }
  bool IsClosed() const { return core_sptr_->IsClosed(); }

 private:
  const std::shared_ptr<grpc_cb_core::ServerWriter> core_sptr_;
};  // class ServerWriter<>

}  // namespace grpc_cb
#endif  // GRPC_CB_SERVER_WRITER_H
