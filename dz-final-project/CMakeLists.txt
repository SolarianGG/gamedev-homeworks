cmake_minimum_required(VERSION 3.16)

project(finalgame CXX C)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(engine SHARED
    ${PROJECT_SOURCE_DIR}/src/engine.cxx
    ${PROJECT_SOURCE_DIR}/src/engine.hxx
    ${PROJECT_SOURCE_DIR}/glad/src/glad.c
    ${PROJECT_SOURCE_DIR}/glad/include/glad/glad.h
    ${PROJECT_SOURCE_DIR}/glad/include/KHR/khrplatform.h
    )

add_executable(game
    ${PROJECT_SOURCE_DIR}/src/game.cxx
    ${PROJECT_SOURCE_DIR}/src/engine.hxx
    )

target_link_libraries(engine PRIVATE SDL2::SDL2 SDL2::SDL2main OpenGL::GL)
target_link_libraries(game PRIVATE engine)
