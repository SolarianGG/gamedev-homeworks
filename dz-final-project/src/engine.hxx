#pragma once
#include <vector>
namespace solar {

struct vec2
{
  vec2();
  vec2(float x, float y);
  float x = 0;
  float y = 0;
};

struct mat2x3
{
  mat2x3();
  static solar::mat2x3 identity();
  static solar::mat2x3 scale(float scale);
  static solar::mat2x3 scale(float sx, float sy);
  static solar::mat2x3 rotation(float thetha);
  static solar::mat2x3 move(const solar::vec2& delta);
  vec2 col0;
  vec2 col1;
  vec2 delta;
};

solar::vec2
operator+(const vec2& l, const vec2& r);

solar::vec2
operator*(const solar::vec2& v, const solar::mat2x3& m);

struct vertex
{
  float x = 0;
  float y = 0;
  float z = 0;
  float r = 0;
  float g = 0;
  float b = 0;
  float a = 0;
};

struct triangle
{
  vertex v[3];
  triangle(vertex v0, vertex v1, vertex v2)
  {
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
  }
};

struct polygon
{
  triangle tr0;
  triangle tr1;
  polygon(triangle tr0, triangle tr1)
    : tr0(tr0)
    , tr1(tr1)
  {}
};

struct event
{
  enum
  {
    pressed,
    reliased,
    quit,
    mouse_motion
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

class engine
{
public:
  virtual ~engine();
  virtual bool init(int window_width, int window_height) = 0;
  virtual bool read_input(solar::event& event) = 0;
  virtual void render(const std::vector<solar::vertex>& vertex_buffer,
                      const solar::mat2x3& m) = 0;
  virtual void swap_buffers() = 0;
  virtual void destroy() = 0;
};
}

solar::engine*
create_engine();

void
destroy_engine(solar::engine* e);
