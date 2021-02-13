#pragma once
#include "03_vertex_buffer_triangle.hxx"

#include <vector>

class indexed_triangle_render : public vertex_buffer_triangle_render
{
  std::vector<int>& index_buffer;

public:
  indexed_triangle_render(image* canvas)
    : vertex_buffer_triangle_render(canvas)
    , index_buffer(*(new std::vector<int>))
  {}
  indexed_triangle_render(image* canvas, std::vector<position>* vertex_buff)
    : vertex_buffer_triangle_render(canvas, vertex_buff)
    , index_buffer(*(new std::vector<int>))
  {}
  indexed_triangle_render(image* canvas, std::vector<int>* index_buff)
    : vertex_buffer_triangle_render(canvas)
    , index_buffer(*index_buff)
  {}

  indexed_triangle_render(image* canvas,
                          std::vector<position>* vertex_buff,
                          std::vector<int>* index_buff)
    : vertex_buffer_triangle_render(canvas, vertex_buff)
    , index_buffer(*index_buff)
  {}
  void add_triangle(position v0, position v1, position v2) override;
  void draw_triangles(const color& rgb) override;
  void clear_buffer() override;
};
