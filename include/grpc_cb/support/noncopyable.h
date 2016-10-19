//  Copy from Boost noncopyable.hpp header file  -------------------//

//  (C) Copyright Beman Dawes 1999-2003. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/utility for documentation.

#ifndef GRPC_CB_SUPPORT_NONCOPYABLE_H
#define GRPC_CB_SUPPORT_NONCOPYABLE_H

//  Deleted copy constructor and copy assignment ensure classes derived from
//  class noncopyable cannot be copied.

//  Contributed by Dave Abrahams

namespace grpc_cb
{
  class noncopyable
  {
  protected:
      constexpr noncopyable() = default;
      ~noncopyable() = default;

      noncopyable( const noncopyable& ) = delete;
      noncopyable& operator=( const noncopyable& ) = delete;
  };
}  // namespace grpc_cb

#endif  // GRPC_CB_SUPPORT_NONCOPYABLE_H
