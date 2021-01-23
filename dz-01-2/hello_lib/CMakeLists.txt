cmake_minimum_required(VERSION 3.16)

project(dzFirst CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(hello_static STATIC include/hello_lib.hxx src/hello_lib.cxx)
target_include_directories(hello_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(hello_dynamic SHARED include/hello_lib.hxx src/hello_lib.cxx)
target_include_directories(hello_dynamic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

