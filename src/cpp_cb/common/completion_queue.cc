/*
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <grpc_cb/impl/completion_queue.h>

#include <cassert>
#include <memory>

#include <grpc/grpc.h>
#include <grpc_cb/support/time.h>

namespace grpc_cb {

CompletionQueue::CompletionQueue()
    : c_cq_uptr_(grpc_completion_queue_create(nullptr),
                 grpc_completion_queue_destroy) {
  assert(c_cq_uptr_);
}

CompletionQueue::CompletionQueue(grpc_completion_queue* take)
    : c_cq_uptr_(take, grpc_completion_queue_destroy) {
  assert(c_cq_uptr_);
}

CompletionQueue::~CompletionQueue() {
  assert(c_cq_uptr_);
}

void CompletionQueue::Shutdown() {
  assert(c_cq_uptr_);
  grpc_completion_queue_shutdown(c_cq_uptr_.get());
}

grpc_event CompletionQueue::NextInternal(gpr_timespec deadline) {
  return grpc_completion_queue_next(c_cq_uptr_.get(), deadline, nullptr);
}

grpc_event CompletionQueue::PluckInternal(void* tag, gpr_timespec deadline) {
  return grpc_completion_queue_pluck(c_cq_uptr_.get(), tag, deadline, nullptr);
}

}  // namespace grpc_cb
