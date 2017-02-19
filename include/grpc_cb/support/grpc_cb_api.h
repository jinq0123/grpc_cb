#ifndef GRPC_CB_SUPPORT_GRPC_CB_API_H
#define GRPC_CB_SUPPORT_GRPC_CB_API_H

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)
// We need to import/export our code only if the user has specifically asked
// for it by defining GRPC_CB_DYN_LINK.
#  if defined(GRPC_CB_DYN_LINK)
// Export if this is our own source, otherwise import.
#    if defined(GRPC_CB_SOURCE)
#      define GRPC_CB_API __declspec(dllexport)
#    else // defined(GRPC_CB_SOURCE)
#      define GRPC_CB_API __declspec(dllimport)
#    endif // defined(GRPC_CB_SOURCE)
#  endif // defined(GRPC_CB_DYN_LINK)
#endif // defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)

#ifndef GRPC_CB_API
#  define GRPC_CB_API
#endif

#endif  // GRPC_CB_SUPPORT_GRPC_CB_API_H
