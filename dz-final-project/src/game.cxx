#include "engine.hxx"
#include <iostream>
solar::mat2x3
operator*(const solar::mat2x3& m1, const solar::mat2x3& m2)
{
  solar::mat2x3 r;

  r.col0.x = m1.col0.x * m2.col0.x + m1.col1.x * m2.col0.y;
  r.col1.x = m1.col0.x * m2.col1.x + m1.col1.x * m2.col1.y;
  r.col0.y = m1.col0.y * m2.col0.x + m1.col1.y * m2.col0.y;
  r.col1.y = m1.col0.y * m2.col1.x + m1.col1.y * m2.col1.y;

  r.delta.x = m1.delta.x * m2.col0.x + m1.delta.y * m2.col0.y + m2.delta.x;
  r.delta.y = m1.delta.x * m2.col1.x + m1.delta.y * m2.col1.y + m2.delta.y;

  return r;
}
int
main(int /*argc*/, char** /*argv*/)
{
  solar::engine* engine = create_engine();
  if (engine == nullptr) {
    std::cerr << "Failed to create engine" << std::endl;
    return 1;
  }

  if (!engine->init(800, 600)) {
    destroy_engine(engine);
    return 2;
  }

  solar::vertex v0 = { -0.3f, -0.3f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
  solar::vertex v1 = { -0.3f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
  solar::vertex v2 = { 0.3f, -0.3f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
  solar::vertex v3 = { 0.3f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };

  std::vector<solar::vertex> vertex_buffer{ v0, v1, v2, v3 };

  solar::vec2 player_pos(0.0f, 0.0f);
  float player_direction(0.0f);
  const float pi = 3.14f;
  bool is_game = true;
  while (is_game) {
    solar::event event;
    while (engine->read_input(event)) {
      switch (event.type) {
        case solar::event::quit: {
          is_game = false;
          break;
        }
        case solar::event::mouse_motion: {
          std::cout << event.mouse_x << " " << event.mouse_y << std::endl;
          break;
        }
        case solar::event::pressed: {
          if (event.key == solar::event::left) {
            player_pos.x -= 0.01f;
            player_direction = pi / 2.f;

          } else if (event.key == solar::event::right) {
            player_pos.x += 0.01f;
            player_direction = -pi / 2.f;
          } else if (event.key == solar::event::top) {
            player_pos.y += 0.01f;
            player_direction = 0.f;

          } else if (event.key == solar::event::bottom) {
            player_pos.y -= 0.01f;
            player_direction = -pi;
          }
        }
        default: {
          break;
        }
      }
    }
    solar::mat2x3 move = solar::mat2x3::move(player_pos);
    solar::mat2x3 aspect = solar::mat2x3::scale(1, 640.f / 480.f);
    solar::mat2x3 rot = solar::mat2x3::rotation(player_direction);
    solar::mat2x3 m = rot * move * aspect;

    engine->render(vertex_buffer, m);
    engine->swap_buffers();
  }
  engine->destroy();
  destroy_engine(engine);
  return 0;
}
