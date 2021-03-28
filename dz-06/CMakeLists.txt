cmake_minimum_required(VERSION 3.16)

project(openglesmedium CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

add_library(engine STATIC
    ${PROJECT_SOURCE_DIR}/src/engine.cxx
    ${PROJECT_SOURCE_DIR}/src/engine.hxx
    ${PROJECT_SOURCE_DIR}/glad/include/glad/glad.h
    ${PROJECT_SOURCE_DIR}/glad/include/KHR/khrplatform.h
    ${PROJECT_SOURCE_DIR}/glad/src/glad.c
    ${PROJECT_SOURCE_DIR}/src/picopng.hxx
    )


add_executable(game
    ${PROJECT_SOURCE_DIR}/src/game.cxx
    )

target_link_libraries(engine PRIVATE SDL2::SDL2 SDL2::SDL2main -lGL -ldl)
target_link_libraries(game PRIVATE engine)
