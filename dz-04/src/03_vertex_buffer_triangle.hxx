#pragma once
#include "00_basic_canvas.hxx"
#include "02_basic_triangle.hxx"

#include <vector>

class vertex_buffer_triangle_render : public triangle_render
{
  std::vector<position>& vertex_buffer;

public:
  vertex_buffer_triangle_render(image* canvas)
    : triangle_render(canvas)
    , vertex_buffer(*(new std::vector<position>))
  {}
  vertex_buffer_triangle_render(image* canvas,
                                std::vector<position>* vertex_buff)
    : triangle_render(canvas)
    , vertex_buffer(*vertex_buff)
  {}
  virtual void add_triangle(position v0, position v1, position v2);
  virtual void draw_triangles(const color& rgb);
  virtual void clear_buffer();

protected:
  std::vector<position>& get_vertex_buffer();
};
