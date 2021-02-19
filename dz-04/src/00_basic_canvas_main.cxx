#include "00_basic_canvas.hxx"
#include <array>
#include <iostream>

const char file_name[] = "00_basic_image.ppm";

int
main(int /*argc*/, char** /*argv*/)
{
  using namespace std;
  color red = { 255, 0, 0 };
  irender rend(new image(320, 240));
  rend.clear(red);
  return rend.get_img().save_image(file_name) ? 0 : 1;
}
