// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_IMPL_CLIENT_CLIENT_SYNC_READER_HELPER_H
#define GRPC_CB_IMPL_CLIENT_CLIENT_SYNC_READER_HELPER_H

#include <cassert>     // for assert()

#include <grpc_cb/impl/call_sptr.h>                              // for CallSptr
#include <grpc_cb/impl/client/client_reader_read_cqtag.h>  // for ClientReaderReadCqTag
#include <grpc_cb/impl/client/client_reader_recv_status_cqtag.h>  // for ClientReaderRecvStatusCqTag
#include <grpc_cb/impl/completion_queue.h>  // for CompletionQueue::Pluck()
#include <grpc_cb/status.h>                 // for Status

namespace grpc_cb {
namespace ClientSyncReaderHelper {
// ClientSyncReaderHelper is used in ClientSyncReader and ClientSyncReaderWriter.

inline bool BlockingReadOne(
    const CallSptr& call_sptr,
    const CompletionQueueSptr& cq_sptr,
    ::google::protobuf::Message& response,
    Status& status);

inline Status BlockingRecvStatus(
    const CallSptr& call_sptr,
    const CompletionQueueSptr& cq_sptr);

// Todo: move to cpp file.

inline bool BlockingReadOne(
    const CallSptr& call_sptr,
    const CompletionQueueSptr& cq_sptr,
    ::google::protobuf::Message& response,
    Status& status) {
  if (!status.ok()) return false;

  ClientReaderReadCqTag tag(call_sptr);
  if (!tag.Start()) {
    status.SetInternalError("End of server stream.");  // Todo: use EndOfStream instead of status.
    return false;
  }

  // tag.Start() has queued the tag. Wait for completion.
  cq_sptr->Pluck(&tag);
  // Todo: check HasGotMsg()...
  status = tag.GetResultMsg(response);
  return status.ok();
}

inline Status BlockingRecvStatus(
    const CallSptr& call_sptr,
    const CompletionQueueSptr& cq_sptr) {
  assert(call_sptr);
  assert(cq_sptr);
  ClientReaderRecvStatusCqTag tag(call_sptr);
  if (!tag.Start())
      return Status::InternalError("Failed to receive status.");
  cq_sptr->Pluck(&tag);
  return tag.GetStatus();
}

}  // namespace ClientSyncReaderHelper
}  // namespace grpc_cb

#endif  // GRPC_CB_IMPL_CLIENT_CLIENT_SYNC_READER_HELPER_H
