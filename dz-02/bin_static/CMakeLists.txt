cmake_minimum_required(VERSION 3.16)

project(sdlhomework CXX)

add_executable(sdl_static ../src/main.cxx)

find_package(SDL2 REQUIRED)
target_include_directories(sdl_static PUBLIC ${SDL2_INCLUDE_DIRS})
#find_library(SDL2_LIB_STATIC
#            NAMES libSDL2d.a
#            PATHS /usr/local /usr/local/lib
#            REQUIRED
#            )
#target_link_libraries(sdl_static ${SDL2_LIB_STATIC} -lrt -lm -ldl -lpthread)
target_link_libraries(sdl_static SDL2::SDL2-static)
