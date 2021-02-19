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

void
interpolated_triangle_render::add_triangle(const vertex v0,
                                           const vertex v1,
                                           const vertex v2)
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

std::vector<position>
interpolated_triangle_render::get_pixels_positions(vertex start_vertex,
                                                   vertex end_vertex)
{
  std::vector<position>* points = new std::vector<position>();

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
    int x = x0;
    int dirx = x1 - x0;
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
interpolated_triangle_render::clear_buffers()
{
  vertex_buffer.clear();
  index_buffer.clear();
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
  if (gfx && prog)
    delete gfx;
  if (prog)
    gfx = prog;
}

std::vector<vertex>
interpolated_triangle_render::rasterize_horizontal_line(
  const vertex left_vertex,
  const vertex right_vertex)
{
  int nums_of_x = abs(right_vertex.pos.x - left_vertex.pos.x);
  std::vector<vertex> pixels;
  if (nums_of_x > 0) {
    for (int i = 0; i < nums_of_x + 1; i++) {
      double t_pixel = static_cast<double>(i) / (nums_of_x + 1);
      vertex pixel = interpolate(left_vertex, right_vertex, t_pixel);

      pixels.push_back(pixel);
    }
  } else {
    pixels.push_back(left_vertex);
  }
  return pixels;
}

std::vector<vertex>
interpolated_triangle_render::rasterize_horizontal_triangle(
  const vertex single_vertex,
  const vertex left_vertex,
  const vertex right_vertex)
{
  std::vector<vertex> pixels;

  int count_of_lines = abs(single_vertex.pos.y - left_vertex.pos.y);

  if (count_of_lines > 0) {
    for (int i = 0; i < count_of_lines + 1; i++) {
      double t_vertical = static_cast<double>(i) / (count_of_lines + 1);
      vertex right = interpolate(right_vertex, single_vertex, t_vertical);

      vertex left = interpolate(left_vertex, single_vertex, t_vertical);

      std::vector<vertex> ver = rasterize_horizontal_line(left, right);

      for (auto l : ver) {
        pixels.push_back(l);
      }
    }

  } else {

    pixels = rasterize_horizontal_line(left_vertex, right_vertex);
  }

  return pixels;
}

std::vector<vertex>
interpolated_triangle_render::rasterize_triangle(const vertex v0,
                                                 const vertex v1,
                                                 const vertex v2)
{
  using namespace std;

  array<const vertex*, 3> vertexes = { &v0, &v1, &v2 };
  sort(begin(vertexes),
       end(vertexes),
       [](const vertex* left, const vertex* right) {
         return left->pos.y < right->pos.y;
       });

  const vertex& top = *vertexes.at(0);
  const vertex& middle = *vertexes.at(1);
  const vertex& bottom = *vertexes.at(2);

  auto top_to_bottop = get_pixels_positions(top, bottom);

  vertex second_middle;
  for (auto l : top_to_bottop) {
    if (l.y == middle.pos.y) {
      second_middle.pos = l;
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

  vector<vertex> out;
  auto top_triangle = rasterize_horizontal_triangle(top, middle, second_middle);
  auto bottom_triangle =
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

    std::vector<vertex> pixels = rasterize_triangle(v0i, v1i, v2i);
    draw_triangle(pixels);
  }
}
