cmake_minimum_required(VERSION 3.16)

project(basicrender CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

add_executable(00_basic_canvas
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.cxx
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.hxx
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas_main.cxx
    )

add_executable(01_basic_line
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.hxx
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.cxx
    ${PROJECT_SOURCE_DIR}/src/01_basic_line.cxx
    ${PROJECT_SOURCE_DIR}/src/01_basic_line.hxx
    ${PROJECT_SOURCE_DIR}/src/01_basic_line_main.cxx
    )

add_executable(02_basic_triangle
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.hxx
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.cxx
    ${PROJECT_SOURCE_DIR}/src/01_basic_line.cxx
    ${PROJECT_SOURCE_DIR}/src/01_basic_line.hxx
    ${PROJECT_SOURCE_DIR}/src/02_basic_triangle.cxx
    ${PROJECT_SOURCE_DIR}/src/02_basic_triangle.hxx
    ${PROJECT_SOURCE_DIR}/src/02_basic_triangle_main.cxx
    )

add_executable(03_vertex_buffer_triangle
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.hxx
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.cxx
    ${PROJECT_SOURCE_DIR}/src/01_basic_line.cxx
    ${PROJECT_SOURCE_DIR}/src/01_basic_line.hxx
    ${PROJECT_SOURCE_DIR}/src/02_basic_triangle.cxx
    ${PROJECT_SOURCE_DIR}/src/02_basic_triangle.hxx
    ${PROJECT_SOURCE_DIR}/src/03_vertex_buffer_triangle.cxx
    ${PROJECT_SOURCE_DIR}/src/03_vertex_buffer_triangle.hxx
    ${PROJECT_SOURCE_DIR}/src/03_vertex_buffer_triangle_main.cxx
    )


add_executable(04_indexed_triangle
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.hxx
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.cxx
    ${PROJECT_SOURCE_DIR}/src/01_basic_line.cxx
    ${PROJECT_SOURCE_DIR}/src/01_basic_line.hxx
    ${PROJECT_SOURCE_DIR}/src/02_basic_triangle.cxx
    ${PROJECT_SOURCE_DIR}/src/02_basic_triangle.hxx
    ${PROJECT_SOURCE_DIR}/src/03_vertex_buffer_triangle.cxx
    ${PROJECT_SOURCE_DIR}/src/03_vertex_buffer_triangle.hxx
    ${PROJECT_SOURCE_DIR}/src/04_indexed_triangle.cxx
    ${PROJECT_SOURCE_DIR}/src/04_indexed_triangle.hxx
    ${PROJECT_SOURCE_DIR}/src/04_indexed_triangle_main.cxx
    )

add_executable(05_interpolated_triangle
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.hxx
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.cxx
    ${PROJECT_SOURCE_DIR}/src/05_interpolated_triangle.cxx
    ${PROJECT_SOURCE_DIR}/src/05_interpolated_triangle.hxx
    ${PROJECT_SOURCE_DIR}/src/05_interpolated_triangle_main.cxx
    )

add_executable(06_my_own_shader
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.hxx
    ${PROJECT_SOURCE_DIR}/src/00_basic_canvas.cxx
    ${PROJECT_SOURCE_DIR}/src/05_interpolated_triangle.cxx
    ${PROJECT_SOURCE_DIR}/src/05_interpolated_triangle.hxx
    ${PROJECT_SOURCE_DIR}/src/06_my_own_shader_main.cxx
    )

target_link_libraries(06_my_own_shader PRIVATE SDL2::SDL2 SDL2::SDL2main)
