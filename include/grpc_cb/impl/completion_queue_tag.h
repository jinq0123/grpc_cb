// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_COMPLETION_QUEUE_TAG_H
#define GRPC_CB_COMPLETION_QUEUE_TAG_H

namespace grpc_cb {

// Interface of grpc_cb completion queue tag. CqTag.
class CompletionQueueTag {
 public:
  virtual ~CompletionQueueTag() {}
  virtual void DoComplete(bool success) {};  // Todo: What is the use of 'success'?
};

}  // namespace grpc_cb

#endif  // GRPC_CB_COMPLETION_QUEUE_TAG_H
