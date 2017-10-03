/*
 *
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

#ifndef GRPC_CB_SUPPORT_SLICE_H
#define GRPC_CB_SUPPORT_SLICE_H

#include <grpc_cb/support/string_ref.h>

namespace grpc_cb {

inline string_ref StringRefFromSlice(const grpc_slice* slice) {
  return string_ref(
      reinterpret_cast<const char*>(GRPC_SLICE_START_PTR(*slice)),
      GRPC_SLICE_LENGTH(*slice));
}

inline std::string StringFromCopiedSlice(grpc_slice slice) {
  return std::string(reinterpret_cast<char*>(GRPC_SLICE_START_PTR(slice)),
                      GRPC_SLICE_LENGTH(slice));
}

inline grpc_slice SliceReferencingString(const std::string& str) {
  return grpc_slice_from_static_buffer(str.data(), str.length());
}

inline grpc_slice SliceFromCopiedString(const std::string& str) {
  return grpc_slice_from_copied_buffer(str.data(), str.length());
}

}  // namespace grpc_cb

#endif  // GRPC_CB_SUPPORT_SLICE_H
