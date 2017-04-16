// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_SERVER_SERVER_METHOD_CALL_CQTAG_H
#define GRPC_CB_SERVER_SERVER_METHOD_CALL_CQTAG_H

#include <cassert>

#include <grpc/grpc.h>          // for grpc_metadata_array
#include <grpc/support/time.h>  // for gpr_timespec

#include <grpc_cb/impl/cqueue_for_next_sptr.h>  // for CQueueForNextSptr
#include <grpc_cb/impl/completion_queue_tag.h>   // for CompletionQueueTag
#include <grpc_cb/support/config.h>              // for GRPC_FINAL

struct grpc_call;

namespace grpc_cb {

class Service;

// Used in class Server.
class ServerMethodCallCqTag GRPC_FINAL : public CompletionQueueTag {
 public:
  // registered_method is the return of grpc_server_register_method().
  ServerMethodCallCqTag(grpc_server* server, Service* service,
                        size_t method_index, void* registered_method,
                        const CQueueForNextSptr& cq4n_sptr);

  virtual ~ServerMethodCallCqTag() GRPC_OVERRIDE;

 public:
  virtual void DoComplete(bool success) GRPC_OVERRIDE;

 private:
  // TODO: No need to copy for each request.
  grpc_server* const server_;
  Service* const service_;
  const size_t method_index_;
  void* const registered_method_;
  const CQueueForNextSptr cq4n_sptr_;

  grpc_call* call_ptr_ = nullptr;
  gpr_timespec deadline_;
  grpc_metadata_array initial_metadata_array_;
  grpc_byte_buffer* payload_ptr_ = nullptr;
};

}  // namespace grpb_cb

#endif  // GRPC_CB_SERVER_SERVER_METHOD_CALL_CQTAG_H
