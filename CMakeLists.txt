project(grpc_cb CXX)
cmake_minimum_required(VERSION 2.8)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

if(MSVC)
    add_definitions(-D_WIN32_WINNT=0x600) # This is a GRPC requirement!!
endif()
include_directories("include")
include_directories(${CONAN_INCLUDE_DIRS})

file(GLOB_RECURSE SRC_LIST "src/cpp_cb/*.cc")  
add_library(grpc_cb ${SRC_LIST})
target_link_libraries(grpc_cb ${CONAN_LIBS})

file(GLOB_RECURSE PLUGIN_SRC_LIST "src/compiler/*.cc")  
add_executable(grpc_cpp_cb_plugin ${PLUGIN_SRC_LIST})
target_link_libraries(grpc_cpp_cb_plugin ${CONAN_LIBS})
