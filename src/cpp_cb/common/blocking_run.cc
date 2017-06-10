// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include <grpc_cb/blocking_run.h>

#include "do_next_completion.h"  // for DoNextCompletion()

#include <grpc_cb/completion_queue_for_next.h>  // for CompletionQueueForNext

namespace grpc_cb {

void BlockingRun(CompletionQueueForNext& cq4n) {
  while (DoNextCompletion(cq4n))
    ;
}

void BlockingRun(const CompletionQueueForNextSptr& cq4n_sptr) {
  if (cq4n_sptr)
    BlockingRun(*cq4n_sptr);
}

}  // namespace grpc_cb
