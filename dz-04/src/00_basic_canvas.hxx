#pragma once
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

#pragma pack(push, 1)
struct color
{
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  friend bool operator==(const color& c2, const color& c);
};
#pragma pack(pop)

const constexpr color black = { 0, 0, 0 };
const constexpr color red = { 255, 0, 0 };
const constexpr color green = { 0, 255, 0 };
const constexpr color blue = { 0, 0, 255 };
const constexpr color white = { 255, 255, 255 };
const constexpr color grey = { 133, 133, 133 };

class image
{
  uint16_t width;
  uint16_t height;
  std::vector<color> pixels;

public:
  image(size_t width, size_t height)
  {
    this->width = width;
    this->height = height;
    for (int i = 0; i < width * height; i++) {
      pixels.push_back(color{ 0, 0, 0 });
    }
  }
  bool save_image(const char* file_name);
  bool load_image(const char* file_name);
  color& get_pixel(size_t x, size_t y);
  void set_pixel(size_t x, size_t y, const color& rgb);
  std::vector<color>& get_pixels();
  bool operator==(const image& canvas) const;
  bool operator!=(const image& canvas) const;
  uint16_t get_width() { return width; }
  uint16_t get_height() { return height; }
};

struct position
{
  int32_t x = 0;
  int32_t y = 0;
  friend bool operator==(const position& p, const position& p1);
  double length() { return std::sqrt(x * x + y * y); }
  friend position operator-(const position& left, const position& right);
};

class irender
{
  image& img;

public:
  irender(image* img)
    : img(*img)
  {}
  void clear(const color& rgb);
  void set_pixel(position& pos, const color& rgb);
  image& get_img();
};
