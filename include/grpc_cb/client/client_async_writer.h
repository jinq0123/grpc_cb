// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_ASYNC_WRITER_H
#define GRPC_CB_CLIENT_ASYNC_WRITER_H

#include <cassert>  // for assert()
#include <cstdint>  // for int64_t
#include <string>

#include <grpc_cb_core/client/client_async_writer.h>  // for grpc_cb_core::ClientAsyncWriter

#include <grpc_cb/client/channel_sptr.h>  // for ChannelSptr
#include <grpc_cb/client/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/common/impl/config.h>   // for GRPC_FINAL
#include <grpc_cb/common/status_fwd.h>    // for Status
#include <grpc_cb/client/impl/wrap_close_cb.h>  // for WrapCloseCb()

namespace grpc_cb {

// Copyable. Thread-safe.
// Use template class instead of template member function
//    to ensure client input the correct request type.
template <class Request, class Response>
class ClientAsyncWriter GRPC_FINAL {
 public:
  inline ClientAsyncWriter(const ChannelSptr& channel,
                           const std::string& method,
                           const CompletionQueueSptr& cq_sptr,
                           int64_t timeout_ms)
      // Todo: same as ClientReader?
      : core_sptr_(new grpc_cb_core::ClientAsyncWriter(
          channel, method, cq_sptr, timeout_ms)) {
    assert(channel);
    assert(cq_sptr);
  }

  // Todo: Get queue size()
  // Todo: SyncGetInitMd();

  bool Write(const Request& request) const {
    return core_sptr_->Write(request.SerializeAsString());
  }

  using CloseCb = std::function<void (const Status&, const Response&)>;
  void Close(const CloseCb& close_cb = CloseCb()) {
    core_sptr_->Close(WrapCloseCb(close_cb));
  }  // Close()

 private:
  // Use non_template class as the implement.
  const std::shared_ptr<grpc_cb_core::ClientAsyncWriter> core_sptr_;  // Easy to copy.
};  // class ClientAsyncWriter<>

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_ASYNC_WRITER_H
