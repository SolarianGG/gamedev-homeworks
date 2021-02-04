#include "engine.hxx"

#include <SDL2/SDL.h>
#include <array>
#include <iostream>
#include <string_view>

// Keyboard keys and variables

enum
{
  unitialize_index = -32000,
  keys_count = 8
};

struct keyboard_bind
{
  SDL_KeyCode key;
  std::string_view button;
};

static std::string_view
check_key(const SDL_Event& event)
{
  std::array<keyboard_bind, keys_count> keys{ { { SDLK_a, "left" },
                                                { SDLK_w, "up" },
                                                { SDLK_s, "down" },
                                                { SDLK_d, "right" },
                                                { SDLK_RETURN, "enter" },
                                                { SDLK_ESCAPE, "quit" },
                                                { SDLK_SPACE, "space" },
                                                { SDLK_LSHIFT, "shift" } } };
  int index = unitialize_index;
  for (int i = 0; i < keys_count; i++) {
    if (keys[i].key == event.key.keysym.sym) {
      index = i;
      break;
    }
  }
  if (index != unitialize_index) {
    return keys[index].button;
  }
  return "";
}

// ------------------------------------------------------

// Additional functions for game

static bool
is_version_equeals(SDL_version* comp, SDL_version* link)
{
  if (comp->major == link->major && comp->minor == link->minor &&
      comp->patch == link->patch)
    return true;
  return false;
}

std::ostream&
operator<<(std::ostream& out, input& e)
{
  out << e.key << " is " << e.event_type;
  return out;
}

// -------------------------------------------------------

// Impl of create_engine and destroy_engine
engine*
create_engine()
{
  SDL_version compiled, linked;
  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);
  if (!is_version_equeals(&compiled, &linked)) {
    std::cerr << "error: using different version of SDL" << std::endl;
    return nullptr;
  }

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "failed to load sdl: " << SDL_GetError() << std::endl;
    return nullptr;
  }

  SDL_Window* window = SDL_CreateWindow("Game",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        800,
                                        600,
                                        SDL_WINDOW_OPENGL);
  if (window == nullptr) {
    std::cerr << "failed to initialize window " << SDL_GetError() << std::endl;
    SDL_Quit();
    return nullptr;
  }

  return new engine();
}

void
destroy_engine(engine* e)
{
  if (e == nullptr)
    return;
  delete e;
}

// --------------------------------------------------------------------

// Impl of class engine methods

engine::engine() {}

bool
engine::read_input(input& e)
{
  using namespace std;
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    e.key = check_key(event);
    if (event.type == SDL_KEYDOWN) {
      e.event_type = ENGINE_KEYBOARD_KEY_PRESSED;
    } else if (event.type == SDL_KEYUP) {
      e.event_type = ENGINE_KEYBOARD_KEY_RELEASED;
    } else if (event.type == SDL_QUIT) {
      e.event_type = ENGINE_QUIT_EVENT;
    }
    return true;
  }
  return false;
}

engine::~engine() {}

// ----------------------------------------------------------------------
