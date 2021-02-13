#pragma once
#include "00_basic_canvas.hxx"

#include <vector>

class line_render : public irender
{
  std::vector<position>& get_way(
    position start,
    position end); // Get all pixels positions for draw_line
public:
  line_render(image* canvas)
    : irender(canvas)
  {}
  void draw_line(position start, position end, const color& rgb);
};
