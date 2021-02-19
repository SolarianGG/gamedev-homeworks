#include "00_basic_canvas.hxx"
#include "01_basic_line.hxx"

const char file_name[] = "01_basic_line.ppm";

int
main(int /*argc*/, char** /*argv*/)
{

  uint16_t width = 320;
  uint16_t height = 240;
  line_render rend(new image(width, height));
  rend.clear(black);
  rend.draw_line(position{ 0, 0 }, position{ 0, height - 1 }, blue);
  rend.draw_line(position{ 0, 0 }, position{ width - 1, 0 }, green);
  rend.draw_line(position{ 0, 0 }, position{ width - 1, height - 1 }, red);
  bool isGood = rend.get_img().save_image(file_name);
  return isGood ? 0 : 1;
}
