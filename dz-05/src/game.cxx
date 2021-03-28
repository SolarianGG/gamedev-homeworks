#include "engine.hxx"
#include <iostream>
int
main(int /*argc*/, char** /*argv*/)
{
  engine* engine = create_engine();
  bool isGood = engine->init();
  if (!isGood) {
    destroy_engine(engine);
    return 1;
  }

  vertex v0 = { 0.3f, -0.3f, -0.5f, 1.0f, 0.f, 0.0f, 1.0f };
  vertex v1 = { -0.3f, -0.3f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f };
  vertex v2 = { 0.0f, 0.3f, -0.5f, 1.0f, 0.f, 0.0f, 1.0f };

  triangle tr;
  tr.v[0] = v0;
  tr.v[1] = v1;
  tr.v[2] = v2;
  vertex v3 = { 0.9f, -0.9f, 0.0f, 1.0f, 0.f, 0.f, 1.0f };
  vertex v4 = { -0.9f, -0.9f, 0.0f, 0.0f, 1.0f, 0.f, 1.0f };
  vertex v5 = { 0.0f, 0.9f, 0.0f, 0.0f, 0.f, 1.0f, 1.0f };

  triangle tr1;
  tr1.v[0] = v3;
  tr1.v[1] = v4;
  tr1.v[2] = v5;
  uniform un = { 0, 0 };
  bool is_loop = true;
  while (is_loop) {
    event e;
    while (engine->read_input(e)) {
      if (e.event_type == event_quit) {
        is_loop = false;
      } else if (e.event_type == event_motion) {
        un.f0 = e.mouse_x;
        un.f1 = e.mouse_y;
      } else if (e.event_type == event_pressed ||
                 e.event_type == event_reliased) {
        std::cout << static_cast<char>(e.key) << " is " << e.event_type
                  << std::endl;
      }
    }
    engine->set_uniforms(un);
    engine->render_triangle(tr);
    engine->render_triangle(tr1);
    engine->swap_buffers();
  }
  engine->destroy();
  destroy_engine(engine);
  return 0;
}
