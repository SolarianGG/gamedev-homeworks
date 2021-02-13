#include "05_interpolated_triangle.hxx"

#include <algorithm>
#include <cmath>
#include <vector>

static int
exists(std::vector<vertex>& arr, vertex value)
{
  using namespace std;
  for (size_t i = 0; i < arr.size(); i++) {
    if (arr.at(i).pos == value.pos) {
      return i;
    }
  }
  return -1;
}

static double
interpolate(const int f0, const int f1, const double t)
{
  return f0 + (f1 - f0) * t;
}

static color
interpolate(const color f0, const color f1, const double t)
{
  color c = { static_cast<uint8_t>(interpolate(f0.r, f1.r, t)),
              static_cast<uint8_t>(interpolate(f0.g, f1.g, t)),
              static_cast<uint8_t>(interpolate(f0.b, f1.b, t)) };
  return c;
}

void
interpolated_triangle_render::add_triangle(const vertex& v0,
                                           const vertex& v1,
                                           const vertex& v2)
{
  int index;
  if ((index = exists(vertex_buffer, v0)) == -1) {
    vertex_buffer.push_back(v0);
    index = exists(vertex_buffer, v0);
  }
  index_buffer.push_back(index);

  if ((index = exists(vertex_buffer, v1)) == -1) {
    vertex_buffer.push_back(v1);
    index = exists(vertex_buffer, v1);
  }
  index_buffer.push_back(index);

  if ((index = exists(vertex_buffer, v2)) == -1) {
    vertex_buffer.push_back(v2);
    index = exists(vertex_buffer, v2);
  }
  index_buffer.push_back(index);
}

std::vector<vertex>&
interpolated_triangle_render::get_pixels_positions(vertex start_vertex,
                                                   vertex end_vertex)
{
  std::vector<vertex>* points = new std::vector<vertex>();

  position start = start_vertex.pos;
  position end = end_vertex.pos;

  int deltaX = abs(end.x - start.x);
  int deltaY = abs(end.y - start.y);

  int error = 0;

  int x0, y0, x1, y1;

  int deltaerr;

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
    int y = y0;
    int diry = y1 - y0;

    if (diry > 0)
      diry = 1;
    else if (diry < 0)
      diry = -1;

    for (int x = x0; x <= x1; x++) {
      position pos;
      pos.x = x;
      pos.y = y;
      points->push_back(vertex{ pos, { 0, 0, 0 } });
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
    int x = x0;
    int dirx = x1 - x0;
    if (dirx > 0)
      dirx = 1;
    else if (dirx < 0)
      dirx = -1;
    for (int y = y0, i = 0; y <= y1; y++, i++) {
      double t_vertical = static_cast<double>(i) / (y1 - y0);
      vertex ver;
      ver.pos = { x, y };
      ver.col = interpolate(end_vertex.col, start_vertex.col, t_vertical);
      points->push_back(ver);
      error += deltaerr;
      if (error >= deltaY + 1) {
        x += dirx;
        error -= deltaY + 1;
      }
    }
  }

  return *points;
}

static vertex
interpolate(const vertex& v0, const vertex& v1, const double t)
{
  vertex ver;
  ver.pos.x = interpolate(v0.pos.x, v1.pos.x, t);
  ver.pos.y = interpolate(v0.pos.y, v1.pos.y, t),
  ver.col.r = interpolate(v0.col.r, v1.col.r, t),
  ver.col.g = interpolate(v0.col.g, v1.col.g, t),
  ver.col.b = interpolate(v0.col.b, v1.col.b, t);
  return ver;
}

void
interpolated_triangle_render::set_gfx_program(shader_program* prog)
{
  if (prog)
    gfx = prog;
}

std::vector<vertex>&
interpolated_triangle_render::rasterize_horizontal_line(
  const vertex& left_vertex,
  const vertex& right_vertex)
{
  auto& pixels = get_pixels_positions(left_vertex, right_vertex);
  if (pixels.size() > 0) {
    for (size_t i = 0; i < pixels.size(); i++) {
      double t_pixel = static_cast<double>(i) / pixels.size();
      vertex pixel = interpolate(left_vertex, right_vertex, t_pixel);

      pixels.at(i) = pixel;
    }
  } else {
    pixels.push_back(left_vertex);
  }
  return pixels;
}

std::vector<vertex>&
interpolated_triangle_render::rasterize_horizontal_triangle(
  const vertex& single_vertex,
  const vertex& left_vertex,
  const vertex& right_vertex)
{
  std::vector<vertex>* pixels = new std::vector<vertex>();

  int count_of_lines = abs(single_vertex.pos.y - left_vertex.pos.y);

  if (count_of_lines > 0) {
    for (int i = 0; i < count_of_lines; i++) {
      double t_vertical = static_cast<double>(i) / count_of_lines;
      vertex right = interpolate(right_vertex, single_vertex, t_vertical);

      vertex left = interpolate(left_vertex, single_vertex, t_vertical);

      std::vector<vertex> ver = rasterize_horizontal_line(left, right);

      for (auto l : ver) {
        pixels->push_back(l);
      }
    }

  } else {
    delete pixels;
    *pixels = rasterize_horizontal_line(left_vertex, right_vertex);
  }

  /* auto& left_line = get_pixels_positions(left_vertex, single_vertex);

   auto& right_line = get_pixels_positions(right_vertex, single_vertex);

   for (int i = 0; i < count_of_lines; i++) {

     const auto left =
       find_if(left_line.begin(), left_line.end(), [&](const vertex& vert) {
         return vert.pos.y == left_line.at(0).pos.y + i;
       });
     const auto right =
       find_if(right_line.begin(), right_line.end(), [&](const vertex& vert) {
         return vert.pos.y == right_line.at(0).pos.y + i;
       });

     std::vector<vertex> ver = rasterize_horizontal_line(*left, *right);

     for (auto l : ver) {
       pixels->push_back(l);
     }
   }*/

  return *pixels;
}

std::vector<vertex>
interpolated_triangle_render::rasterize_triangle(const vertex& v0,
                                                 const vertex& v1,
                                                 const vertex& v2)
{
  using namespace std;
  vector<vertex> out;

  array<const vertex*, 3> vertexes = { &v0, &v1, &v2 };
  sort(begin(vertexes),
       end(vertexes),
       [](const vertex* left, const vertex* right) {
         return left->pos.y < right->pos.y;
       });

  const vertex& top = *vertexes.at(0);
  const vertex& middle = *vertexes.at(1);
  const vertex& bottom = *vertexes.at(2);

  auto& top_to_bottop = get_pixels_positions(top, bottom);

  vertex second_middle;
  for (auto l : top_to_bottop) {
    if (l.pos.y == middle.pos.y) {
      second_middle = l;
      break;
    }
  }

  double t{ 0 };
  double end_start = (bottom.pos - top.pos).length();
  if (end_start > 0) {
    double middle_start = (second_middle.pos - top.pos).length();
    t = middle_start / end_start;
  }
  second_middle = interpolate(top, bottom, t);

  auto& top_triangle =
    rasterize_horizontal_triangle(top, middle, second_middle);
  auto& bottom_triangle =
    rasterize_horizontal_triangle(bottom, middle, second_middle);

  out.insert(out.end(), top_triangle.begin(), top_triangle.end());
  out.insert(out.end(), bottom_triangle.begin(), bottom_triangle.end());
  return out;
}

void
interpolated_triangle_render::draw_triangle(const std::vector<vertex>& pixels)
{
  for (auto l : pixels) {
    l.col = gfx->fragment_shader(l);
    set_pixel(l.pos, l.col);
  }
}

void
interpolated_triangle_render::draw_triangles()
{
  for (size_t i = 0; i < index_buffer.size() / 3; i++) {
    uint16_t index0 = index_buffer.at(i * 3 + 0);
    uint16_t index1 = index_buffer.at(i * 3 + 1);
    uint16_t index2 = index_buffer.at(i * 3 + 2);

    const vertex v0 = vertex_buffer.at(index0);
    const vertex v1 = vertex_buffer.at(index1);
    const vertex v2 = vertex_buffer.at(index2);

    const vertex v0i = gfx->vertex_shader(v0);
    const vertex v1i = gfx->vertex_shader(v1);
    const vertex v2i = gfx->vertex_shader(v2);

    const std::vector<vertex>& pixels = rasterize_triangle(v0i, v1i, v2i);
    draw_triangle(pixels);
  }
}
