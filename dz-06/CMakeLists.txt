cmake_minimum_required(VERSION 3.16)

project(openglesmedium CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
add_library(engine_06 STATIC
    ${PROJECT_SOURCE_DIR}/src/engine.cxx
    ${PROJECT_SOURCE_DIR}/src/engine.hxx
    ${PROJECT_SOURCE_DIR}/glad/include/glad/glad.h
    ${PROJECT_SOURCE_DIR}/glad/include/KHR/khrplatform.h
    ${PROJECT_SOURCE_DIR}/glad/src/glad.c
    ${PROJECT_SOURCE_DIR}/src/picopng.hxx
    )


add_executable(game_06
    ${PROJECT_SOURCE_DIR}/src/game.cxx
    )

target_link_libraries(engine_06 PRIVATE SDL2::SDL2 SDL2::SDL2main OpenGL::GL)
target_link_libraries(game_06 PRIVATE engine_06)
