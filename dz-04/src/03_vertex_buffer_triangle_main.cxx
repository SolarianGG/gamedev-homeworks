#include "00_basic_canvas.hxx"
#include "03_vertex_buffer_triangle.hxx"

const char file_name[] = "03_vertex_triangles.ppm";

int
main(int /*argc*/, char** /*argv*/)
{
  uint16_t width = 320;
  uint16_t height = 240;
  vertex_buffer_triangle_render rend(new image(width, height));
  rend.clear(black);

  int max = 10;

  for (int i = 0; i < max; i++) {
    for (int j = 0; j < max; j++) {
      int32_t step_x = (width - 1) / max;
      int32_t step_y = (height - 1) / max;

      position v0{ i * step_x, j * step_y };
      position v1{ v0.x + step_x, v0.y };
      position v2{ v0.x, v0.y + step_y };
      position v3{ v1.x, v2.y };

      rend.add_triangle(v0, v1, v2);
      rend.add_triangle(v2, v1, v3);
    }
  }
  rend.draw_triangles(red);
  rend.clear_buffer();
  bool isGood = rend.get_img().save_image(file_name);
  return isGood ? 0 : 1;
}
