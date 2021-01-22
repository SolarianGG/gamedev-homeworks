cmake_minimum_required(VERSION 3.16)

project(dzFirstOne CXX)

add_executable(main main.cxx)
target_compile_features(main PUBLIC cxx_std_17)
include(CTest)

add_test(NAME check_hello_exit_zero
         COMMAND 01-1-hello-world
         )

