// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_CLIENT_ASYNC_READER_IMPL_H
#define GRPC_CB_IMPL_CLIENT_CLIENT_ASYNC_READER_IMPL_H

#include <mutex>
#include <string>

#include <grpc_cb/impl/atomic_bool_sptr.h>  // for AtomicBoolSptr
#include <grpc_cb/impl/call_sptr.h>         // for CallSptr
#include <grpc_cb/impl/channel_sptr.h>      // for ChannelSptr
#include <grpc_cb/impl/client/client_async_read_handler_sptr.h>  // for ClientAsyncReadHandlerSptr
#include <grpc_cb/impl/completion_queue_sptr.h>  // for CompletionQueueSptr
#include <grpc_cb/status.h>                      // for Status
#include <grpc_cb/status_callback.h>             // for StatusCallback
#include <grpc_cb/support/config.h>              // for GRPC_FINAL
#include <grpc_cb/support/protobuf_fwd.h>        // for Message

namespace grpc_cb {

class ClientAsyncReaderHelper;

// Thread-safe.
class ClientAsyncReaderImpl GRPC_FINAL {
 public:
  ClientAsyncReaderImpl(const ChannelSptr& channel,
                        const std::string& method,
                        const ::google::protobuf::Message& request,
                        const CompletionQueueSptr& cq_sptr);
  ~ClientAsyncReaderImpl();

 public:
  // ReadHandler and OnStatus must be set before Start().
  void SetReadHandler(const ClientAsyncReadHandlerSptr& handler);
  void SetOnStatus(const StatusCallback& on_status);
  void Start();

  // Todo: Stop reading any more...

 private:
  std::mutex mtx_;
  using Guard = std::lock_guard<std::mutex>;

  const CompletionQueueSptr cq_sptr_;
  const CallSptr call_sptr_;
  const AtomicBoolSptr status_ok_sptr_;  // Shared in ReaderHelper
  Status status_;

  ClientAsyncReadHandlerSptr read_handler_sptr_;
  StatusCallback on_status_;

  // ReaderHelper will be shared by CqTag.
  std::shared_ptr<ClientAsyncReaderHelper> reader_sptr_;
};  // class ClientAsyncReaderImpl

}  // namespace grpc_cb
#endif  // GRPC_CB_IMPL_CLIENT_CLIENT_ASYNC_READER_IMPL_H
