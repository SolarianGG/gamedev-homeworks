#pragma once
#include <cstddef>
#include <string_view>
#include <vector>
struct vec2
{
  vec2();
  vec2(float x, float y);
  float x = 0;
  float y = 0;
};

vec2
operator+(const vec2& l, const vec2& r);

struct mat2x3
{
  mat2x3();
  static mat2x3 identity();
  static mat2x3 scale(float scale);
  static mat2x3 scale(float sx, float sy);
  static mat2x3 rotation(float thetha);
  static mat2x3 move(const vec2& delta);
  vec2 col0;
  vec2 col1;
  vec2 delta;
};

vec2
operator*(const vec2& v, const mat2x3& m);
mat2x3
operator*(const mat2x3& m1, const mat2x3& m2);

struct event
{
  enum
  {
    pressed,
    reliased,
    quit,
    mouse_motion,
    mouse_click
  } type;
  enum
  {
    left,
    right,
    top,
    bottom,
    enter,
    escape,
    space
  } key;
  int mouse_x = 0;
  int mouse_y = 0;
};

struct uniform
{
  int f0;
  int f1;
};

struct vertex
{
  float x = 0.f;
  float y = 0.f;
  float z = 0.f;
  float r = 0.f;
  float g = 0.f;
  float b = 0.f;
  float a = 0.f;
  float tx = 0.f;
  float ty = 0.f;
};

struct triangle
{
  vertex v[3];
};

class engine
{
public:
  virtual ~engine();
  virtual bool init() = 0;
  virtual bool read_input(event& e) = 0;
  virtual bool load_texture(std::string_view path, uint32_t tex_handl) = 0;
  virtual bool load_sound(std::string_view path, uint32_t sound_handl) = 0;
  virtual void render(const triangle& t) = 0;
  virtual void render(const std::vector<vertex>& vertex_buffer,
                      const mat2x3& m,
                      uint32_t texture_location) = 0;
  virtual void set_uniforms(const uniform& un) = 0;
  virtual void swap_buffers() = 0;
  virtual void destroy() = 0;
};

engine*
create_engine();
void
destroy_engine(engine* e);
