// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_INTERNAL_CPP_CB_IMPL_METADATA_VECTOR_H
#define GRPC_INTERNAL_CPP_CB_IMPL_METADATA_VECTOR_H

#include <vector>

#include <grpc/grpc.h>                     // for grpc_metadata

namespace grpc_cb {

using MetadataVector = std::vector<grpc_metadata>;

}  // namespace grpc_cb

#endif  // GRPC_INTERNAL_CPP_CB_IMPL_METADATA_VECTOR_H
