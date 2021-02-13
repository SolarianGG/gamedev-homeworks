#include "00_basic_canvas.hxx"
#include "05_interpolated_triangle.hxx"

#include <vector>

struct gfx_shader : public shader_program
{
public:
  void set_uniforms(const uniforms& un) override {}
  vertex vertex_shader(const vertex& v) override { return v; }
  color fragment_shader(const vertex& v) override { return v.col; }
};

int
main(int /*argc*/, char** /*argv*/)
{

  interpolated_triangle_render render;
  render.set_gfx_program(new gfx_shader());
  vertex v0 = { position{ 160, 20 }, red };
  vertex v1 = { position{ 80, 180 }, green };
  vertex v2 = { position{ 240, 220 }, blue };
  render.add_triangle(v0, v1, v2);
  render.clear(black);
  render.draw_triangles();
  bool isGood = render.get_img().save_image("05_interpolated_triangle.ppm");
  return isGood ? 0 : 1;
}
