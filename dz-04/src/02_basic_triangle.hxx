#pragma once
#include "00_basic_canvas.hxx"
#include "01_basic_line.hxx"

class triangle_render : public line_render
{
public:
  triangle_render(image* canvas)
    : line_render(canvas)
  {}
  void draw_triangle(position v0, position v1, position v2, const color& rgb);
};
