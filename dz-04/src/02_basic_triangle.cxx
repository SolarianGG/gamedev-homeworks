#include "02_basic_triangle.hxx"
#include "00_basic_canvas.hxx"
#include "01_basic_line.hxx"

#include <vector>

void
triangle_render::draw_triangle(position v0,
                               position v1,
                               position v2,
                               const color& rgb)
{
  draw_line(v0, v2, rgb);
  draw_line(v0, v1, rgb);
  draw_line(v1, v2, rgb);
}
