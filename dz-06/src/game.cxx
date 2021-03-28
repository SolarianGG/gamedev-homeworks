#include "engine.hxx"
#include <cmath>
#include <fstream>

#include <iostream>

std::istream&
operator>>(std::istream& is, vertex& v)
{
  is >> v.x;
  is >> v.y;
  is >> v.z;
  is >> v.r;
  is >> v.g;
  is >> v.b;
  is >> v.a;
  return is;
}

std::istream&
operator>>(std::istream& is, triangle& t)
{
  is >> t.v[0];
  is >> t.v[1];
  is >> t.v[2];
  return is;
}

vertex
blend_vertex(const vertex& vl, const vertex& vr, const float a)
{
  vertex r;
  r.x = (1.0f - a) * vl.x + a * vr.x;
  r.y = (1.0f - a) * vl.y + a * vr.y;
  r.r = vl.r;
  r.g = vl.g;
  r.b = vl.b;
  r.a = vl.a;
  return r;
}

triangle
blend(const triangle& tl, const triangle& tr, const float a)
{
  triangle r;
  r.v[0] = blend_vertex(tl.v[0], tr.v[0], a);
  r.v[1] = blend_vertex(tl.v[1], tr.v[1], a);
  r.v[2] = blend_vertex(tl.v[2], tr.v[2], a);
  return r;
}

int
main(int /*argc*/, char** /*argv*/)
{
  engine* engine = create_engine();
  if (!engine->init()) {
    destroy_engine(engine);
    return 1;
  }

  triangle tr1;
  triangle tr2;

  vertex v0 = { 0.5f, -0.5f, 1.0f, 1.0f };
  vertex v1 = { -0.5f, -0.5f, 0.0f, 1.0f };
  vertex v2 = { -0.5f, 0.5f, 0.0f, 0.0f };
  tr1.v[0] = v0;
  tr1.v[1] = v1;
  tr1.v[2] = v2;
  vertex v3 = { 0.5f, 0.5f, 1.0f, 0.0f };
  tr2.v[0] = v0;
  tr2.v[1] = v3;
  tr2.v[2] = v2;

  glm::vec3 tank_pos(0.0f, 0.0f, 0.0f);
  glm::mat4x4 trans;

  std::vector<vertex> vertex_buffer{ v0, v1, v2, v3 };

  bool is_loop = true;
  while (is_loop) {
    event e;
    while (engine->read_input(e)) {
      if (e.event_type == event_quit) {
        is_loop = false;
      } else {
        if (e.event_type == event_pressed) {
          if (e.key == 'w') {
            tank_pos.y += 0.01f;
          } else if (e.key == 's') {
            tank_pos.y -= 0.01f;
          } else if (e.key == 'd') {
            tank_pos.x += 0.01f;
          } else if (e.key == 'a') {
            tank_pos.x -= 0.01f;
          }
        }
        std::cout << static_cast<char>(e.key) << " is " << e.event_type
                  << std::endl;
      }
    }

    trans = glm::translate(trans, tank_pos);

    engine->render_triangle(vertex_buffer, trans);
    engine->swap_buffers();
  }
  engine->destroy();
  destroy_engine(engine);
  return 0;
}
