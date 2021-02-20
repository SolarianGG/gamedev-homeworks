#pragma once
#include "00_basic_canvas.hxx"
#include <vector>

struct vertex
{
  position pos = { 0, 0 };
  color col = { 0, 0, 0 };
  double u = 0.0;
  double v = 0.0;
};

struct edges
{
  position f0 = { 0, 0 };
  position f1 = { 0, 0 };
  position f2 = { 0, 0 };
  position f3 = { 0, 0 };
  position f4 = { 0, 0 };
  position f5 = { 0, 0 };
  position f6 = { 0, 0 };
  position f7 = { 0, 0 };
  position f8 = { 0, 0 };
};

struct uniforms
{
  int f0 = 0;
  int f1 = 0;
  int f2 = 0;
  int f3 = 0;
  int f4 = 0;
  int f5 = 0;
  int f6 = 0;
  int f7 = 0;
  edges edg;
  uint32_t event_type;
};

class shader_program
{
public:
  virtual void set_uniforms(const uniforms&) = 0;
  virtual vertex vertex_shader(const vertex&) = 0;
  virtual color fragment_shader(const vertex&) = 0;
  virtual ~shader_program() = default;
};

class interpolated_triangle_render : public irender
{
  shader_program* gfx;
  std::vector<vertex> vertex_buffer;
  std::vector<uint16_t> index_buffer;

public:
  interpolated_triangle_render(
    image* canvas = new image(320, 240),
    shader_program* sh = nullptr,
    std::vector<vertex>* vertex_buffer = new std::vector<vertex>,
    std::vector<uint16_t>* index_buffer = new std::vector<uint16_t>)
    : irender(canvas)
    , gfx(sh)
    , vertex_buffer(*vertex_buffer)
    , index_buffer(*index_buffer)
  {}

  void draw_triangle(const std::vector<vertex>& pixels);
  void add_triangle(const vertex v0, const vertex v1, const vertex v2);
  void set_gfx_program(shader_program* prog);
  void clear_buffers();
  void draw_triangles();

private:
  std::vector<position> get_pixels_positions(vertex start, vertex end);
  std::vector<vertex> rasterize_horizontal_line(const vertex left_vertex,
                                                const vertex right_vertex);

  std::vector<vertex> rasterize_horizontal_triangle(const vertex single_vertex,
                                                    const vertex left_vertex,
                                                    const vertex right_vertex);

  std::vector<vertex> rasterize_triangle(const vertex v0,
                                         const vertex v1,
                                         const vertex v2);
};
