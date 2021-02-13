#include "01_basic_line.hxx"
#include "00_basic_canvas.hxx"

#include <cmath>
#include <vector>

std::vector<position>&
line_render::get_way(position start, position end)
{
  std::vector<position>* points = new std::vector<position>();

  int64_t deltaX = abs(end.x - start.x);
  int64_t deltaY = abs(end.y - start.y);

  int64_t error = 0;

  int64_t x0, y0, x1, y1;

  int64_t deltaerr;

  if (deltaX >= deltaY) {

    if (start.x < end.x) {
      x0 = start.x;
      y0 = start.y;
      x1 = end.x;
      y1 = end.y;
    } else {
      x0 = end.x;
      y0 = end.y;
      x1 = start.x;
      y1 = start.y;
    }

    deltaerr = deltaY + 1;
    int64_t y = y0;
    int64_t diry = y1 - y0;

    if (diry > 0)
      diry = 1;
    else if (diry < 0)
      diry = -1;

    for (int x = x0; x <= x1; x++) {
      position pos;
      pos.x = x;
      pos.y = y;
      points->push_back(pos);
      error += deltaerr;
      if (error >= deltaX + 1) {
        y += diry;
        error -= deltaX + 1;
      }
    }

  } else {
    if (start.y < end.y) {
      x0 = start.x;
      y0 = start.y;
      x1 = end.x;
      y1 = end.y;
    } else {
      x0 = end.x;
      y0 = end.y;
      x1 = start.x;
      y1 = start.y;
    }

    deltaerr = deltaX + 1;
    int64_t x = x0;
    int64_t dirx = x1 - x0;
    if (dirx > 0)
      dirx = 1;
    else if (dirx < 0)
      dirx = -1;
    for (int y = y0; y <= y1; y++) {
      position pos;
      pos.x = x;
      pos.y = y;
      points->push_back(pos);
      error += deltaerr;
      if (error >= deltaY + 1) {
        x += dirx;
        error -= deltaY + 1;
      }
    }
  }

  return *points;
}

void
line_render::draw_line(position start, position end, const color& rgb)
{
  std::vector<position>& pos = get_way(start, end);
  for (auto l : pos) {
    set_pixel(l, rgb);
  }
}
