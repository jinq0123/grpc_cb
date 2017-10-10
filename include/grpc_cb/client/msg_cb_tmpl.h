// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)
#ifndef GRPC_CB_CLIENT_MSG_CB_TMPL_H
#define GRPC_CB_CLIENT_MSG_CB_TMPL_H

#include <functional>

namespace grpc_cb {

// Message callback function template.
template <class Msg>
using MsgCbTmpl = std::function<void (const Msg& msg)>;

}  // namespace grpc_cb
#endif  // GRPC_CB_CLIENT_MSG_CB_TMPL_H
