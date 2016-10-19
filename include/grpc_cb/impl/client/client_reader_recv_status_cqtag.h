// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_READER_RECV_STATUS_CQTAG_H
#define GRPC_CB_CLIENT_READER_RECV_STATUS_CQTAG_H

#include <grpc/support/port_platform.h>  // for GRPC_MUST_USE_RESULT

#include <grpc_cb/impl/call_cqtag.h>       // for CallCqTag
#include <grpc_cb/impl/call_op_data.h>     // for CodClientRecvStatus
#include <grpc_cb/impl/call_operations.h>  // for CallOperations
#include <grpc_cb/impl/call_sptr.h>        // for CallSptr

namespace grpc_cb {

// Recv status for ClientReader and ClientReaderWriter.
// Base class of ClientReaderAsyncReadStatusCqTag.
class ClientReaderRecvStatusCqTag : public CallCqTag {
 public:
  explicit ClientReaderRecvStatusCqTag(const CallSptr& call_sptr)
      : CallCqTag(call_sptr) {
    assert(call_sptr);
  }
  virtual ~ClientReaderRecvStatusCqTag() {}

 public:
  inline bool Start() GRPC_MUST_USE_RESULT;

public:
  Status GetStatus() {
    // Todo: Get trailing metadata.
    return cod_client_recv_status_.GetStatus();
  }

 private:
  CodClientRecvStatus cod_client_recv_status_;
};  // class ClientReaderRecvStatusCqTag

bool ClientReaderRecvStatusCqTag::Start() {
  CallOperations ops;
  ops.ClientRecvStatus(cod_client_recv_status_);
  return GetCallSptr()->StartBatch(ops, this);
}

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_READER_RECV_STATUS_CQTAG_H
