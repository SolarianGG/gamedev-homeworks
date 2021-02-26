#pragma once

#include <SDL2/SDL_video.h>
#include <string_view>

struct input
{
  std::string_view key;
  std::string_view event_type;
};

class engine
{
  SDL_Window* window = nullptr;

public:
  engine(SDL_Window* window);
  bool read_input(input& e);
  void render_triangle();
  void swap_window();
  ~engine();
};

std::ostream&
operator<<(std::ostream& out, input& e);

class game
{
public:
  virtual void on_event(input& in) = 0;
  virtual ~game() = default;
};

extern "C" game*
create_game(engine* e);

extern "C" void
delete_game(game* g);
