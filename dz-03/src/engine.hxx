#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <string_view>

#define ENGINE_QUIT_EVENT "engine_quit"
#define ENGINE_KEYBOARD_KEY_PRESSED "pressed"
#define ENGINE_KEYBOARD_KEY_RELEASED "released"

class engine;

struct input
{
  std::string_view key;
  std::string_view event_type;
};

std::ostream&
operator<<(std::ostream& out, input& e);

engine*
create_engine();
void
destroy_engine(engine* e);

class engine
{
public:
  engine();
  bool read_input(input& e);
  ~engine();
};
