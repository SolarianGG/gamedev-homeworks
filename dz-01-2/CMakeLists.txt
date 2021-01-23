cmake_minimum_required(VERSION 3.16)

project(dzFirst CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(hello_lib)
add_subdirectory(hello_bin)
