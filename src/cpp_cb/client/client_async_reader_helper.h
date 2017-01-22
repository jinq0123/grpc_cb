// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_ASYNC_READER_HELPER_H
#define GRPC_CB_CLIENT_CLIENT_ASYNC_READER_HELPER_H

#include <atomic>  // for atomic_bool
#include <functional>
#include <memory>  // for enable_shared_from_this<>

#include <grpc_cb/impl/call_sptr.h>         // for CallSptr
#include <grpc_cb/impl/client/client_async_read_handler_sptr.h>  // for ClientAsyncReadHandlerSptr
#include <grpc_cb/status.h>                      // for Status
#include <grpc_cb/support/config.h>              // for GRPC_FINAL

#include "client_async_reader_helper_sptr.h"

namespace grpc_cb {

class ClientReaderAsyncReadCqTag;

// Used in ClientAsyncReader and ClientAsyncReaderWriter.
class ClientAsyncReaderHelper GRPC_FINAL
    : public std::enable_shared_from_this<ClientAsyncReaderHelper> {
 public:
  using OnEnd = std::function<void()>;
  ClientAsyncReaderHelper(CallSptr call_sptr,
                          const ClientAsyncReadHandlerSptr& read_handler_sptr,
                          const OnEnd& on_end);
  ~ClientAsyncReaderHelper();

 public:
  void Start();
  void Abort() { aborted_ = true; }
  const Status& GetStatus() const { return status_; }

 public:
  // for ClientReaderAsyncReadCqTag
  void OnRead(ClientReaderAsyncReadCqTag& tag);
  // DEL CallSptr GetCallSptr() const { return call_sptr_; }

 private:
  void Next();

 private:
  const CallSptr call_sptr_;
  std::atomic_bool aborted_{ false };  // abort reader
  Status status_;
  bool started_{ false };

  const ClientAsyncReadHandlerSptr read_handler_sptr_;
  const OnEnd on_end_;
};  // ClientAsyncReaderHelper

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_CLIENT_ASYNC_READER_HELPER_H
