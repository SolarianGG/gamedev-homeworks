cmake_minimum_required(VERSION 3.16)

project(dzFirst CXX)

add_executable(hello_bin_static src/main.cxx)
target_compile_features(hello_bin_static PRIVATE cxx_std_17)
target_link_libraries(hello_bin_static LINK_PUBLIC hello_static)
target_link_options(hello_bin_static PRIVATE -static)

add_executable(hello_bin_dynamic src/main.cxx)
target_compile_features(hello_bin_dynamic PRIVATE cxx_std_17)
target_link_libraries(hello_bin_dynamic LINK_PUBLIC hello_dynamic)
