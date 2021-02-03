cmake_minimum_required(VERSION 3.16)

project(sdlenginegame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

add_executable(game ${PROJECT_SOURCE_DIR}/src/game.cxx)
target_link_libraries(game PRIVATE SDL2::SDL2 SDL2::SDL2main)
