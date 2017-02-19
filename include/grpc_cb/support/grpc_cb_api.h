#ifndef GRPC_CB_SUPPORT_GRPC_CB_API_H
#define GRPC_CB_SUPPORT_GRPC_CB_API_H

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)
#  if defined(GRPC_CB_DLL_EXPORT)
#    define GRPC_CB_API __declspec(dllexport)
#  elif defined(GRPC_CB_DLL_IMPORT)
#    define GRPC_CB_API __declspec(dllimport)
#  endif
#endif // defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)

#ifndef GRPC_CB_API
#  define GRPC_CB_API
#endif

#endif  // GRPC_CB_SUPPORT_GRPC_CB_API_H
