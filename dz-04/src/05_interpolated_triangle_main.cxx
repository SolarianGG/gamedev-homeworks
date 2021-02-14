#include "00_basic_canvas.hxx"
#include "05_interpolated_triangle.hxx"

#include <vector>

struct gfx_shader : public shader_program
{
  uniforms un;

public:
  void set_uniforms(const uniforms& un) override { this->un = un; }
  vertex vertex_shader(const vertex& v) override
  {
    vertex out = v;
    out.pos.x *= 1.2;
    return out;
  }
  color fragment_shader(const vertex& v) override
  {
    color c = v.col;
    c.r *= 1.2;
    c.g *= 1.2;
    c.b *= 1.5;
    return c;
  }
};

int
main(int /*argc*/, char** /*argv*/)
{
  interpolated_triangle_render render;
  render.set_gfx_program(new gfx_shader());
  vertex v0 = { position{ 160, 220 }, red };
  vertex v1 = { position{ 80, 180 }, green };
  vertex v2 = { position{ 240, 220 }, blue };
  render.add_triangle(v0, v1, v2);
  render.clear(black);
  render.draw_triangles();
  render.clear_buffers();
  bool isGood = render.get_img().save_image("05_interpolated_triangle.ppm");
  return isGood ? 0 : 1;
}
