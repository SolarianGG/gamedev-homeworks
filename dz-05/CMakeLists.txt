cmake_minimum_required(VERSION 3.16)

project(openglesminimal CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(engine_05 STATIC
    ${PROJECT_SOURCE_DIR}/src/engine.cxx
    ${PROJECT_SOURCE_DIR}/src/engine.hxx
    ${PROJECT_SOURCE_DIR}/glad/include/glad/glad.h
    ${PROJECT_SOURCE_DIR}/glad/include/KHR/khrplatform.h
    ${PROJECT_SOURCE_DIR}/glad/src/glad.c
    )


add_executable(game_05
    ${PROJECT_SOURCE_DIR}/src/game.cxx
    )

target_link_libraries(engine_05 PRIVATE SDL2::SDL2 SDL2::SDL2main OpenGL::GL)
target_link_libraries(game_05 PRIVATE engine_05)
