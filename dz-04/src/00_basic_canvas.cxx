#include "00_basic_canvas.hxx"

#include <array>
#include <fstream>
#include <string_view>

bool
operator==(const color& c, const color& c2)
{
  if (c2.r == c.r && c2.g == c.g && c2.b == c.b)
    return true;
  return false;
}

bool
operator==(const position& p, const position& p1)
{
  if (p.x == p1.x && p.y == p1.y)
    return true;
  return false;
}

position
operator-(const position& left, const position& right)
{
  return { left.x - right.x, left.y - right.y };
}

// Impl of image methods

bool
image::save_image(const char* file_name)
{
  using namespace std;
  ofstream f;
  f.open(file_name, ios::binary);
  f << "P6\n" << width << ' ' << height << '\n' << 255 << '\n';
  auto buff_size = sizeof(color) * this->pixels.size();
  f.write(reinterpret_cast<const char*>(&this->pixels), buff_size);
  f.close();
  return f.good() ? true : false;
}

bool
image::load_image(const char* file_name)
{
  using namespace std;
  ifstream f;
  f.open(file_name, ios::binary);
  if (!f.is_open()) {
    perror(file_name);
    return false;
  }
  auto buff_size = sizeof(color) * this->pixels.size();
  f.read(reinterpret_cast<char*>(&this->pixels), buff_size);
  f.close();
  return f.good() ? true : false;
}

color&
image::get_pixel(size_t x, size_t y)
{
  return this->pixels.at(width * y + x);
}

void
image::set_pixel(size_t x, size_t y, const color& rgb)
{
  get_pixel(x, y) = rgb;
}

std::array<color, buffer_size>&
image::get_pixels()
{
  return this->pixels;
}

bool
image::operator==(const image& canvas) const
{
  return this->pixels == canvas.pixels;
}

bool
image::operator!=(const image& canvas) const
{
  return !(*this == canvas);
}

// ----------------------------------------

// Impl of irender methods

void
irender::clear(const color& rgb)
{
  auto& canvas = img.get_pixels();
  std::fill(begin(canvas), end(canvas), rgb);
}

void
irender::set_pixel(position& pos, const color& rgb)
{
  img.set_pixel(pos.x, pos.y, rgb);
}

image&
irender::get_img()
{
  return img;
}

// ----------------------------------------
