cmake_minimum_required(VERSION 3.16)

project(sdlhomework CXX)

add_executable(sdl_dynamic ../src/main.cxx)

find_package(SDL2 REQUIRED)
#target_include_directories(sdl_dynamic PUBLIC ${SDL2_INCLUDE_DIRS})
#find_library(SDL2_LIB_DYNAMIC
#            NAMES libSDL2d.so
#            PATHS /usr/local /usr/local/lib
#            REQUIRED
#            )
#target_link_libraries(sdl_dynamic ${SDL2_LIB_DYNAMIC})
target_link_libraries(sdl_dynamic SDL2::SDL2)


