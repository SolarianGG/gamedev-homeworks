#include "03_vertex_buffer_triangle.hxx"
#include "00_basic_canvas.hxx"

#include <vector>

// Impl of vertex_buffer_triangle_render methods

void
vertex_buffer_triangle_render::draw_triangles(const color& rgb)
{
  for (size_t i = 0; i < vertex_buffer.size() / 3; i++) {
    position v0 = vertex_buffer.at(3 * i + 0);
    position v1 = vertex_buffer.at(3 * i + 1);
    position v2 = vertex_buffer.at(3 * i + 2);
    draw_triangle(v0, v1, v2, rgb);
  }
}

void
vertex_buffer_triangle_render::clear_buffer()
{
  vertex_buffer.clear();
}

void
vertex_buffer_triangle_render::add_triangle(position v0,
                                            position v1,
                                            position v2)
{
  vertex_buffer.push_back(v0);
  vertex_buffer.push_back(v1);
  vertex_buffer.push_back(v2);
}

std::vector<position>&
vertex_buffer_triangle_render::get_vertex_buffer()
{
  return vertex_buffer;
}

// ---------------------------------------------
