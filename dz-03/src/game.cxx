#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>

#include <array>
#include <iostream>
#include <string_view>

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

void
read_input(const SDL_Event& event)
{
  const std::array<keyboard_bind, keys_count> keys{ { { SDLK_a, "left" },
                                                      { SDLK_w, "up" },
                                                      { SDLK_s, "down" },
                                                      { SDLK_d, "right" },
                                                      { SDLK_RETURN, "enter" },
                                                      { SDLK_ESCAPE, "quit" },
                                                      { SDLK_SPACE, "space" },
                                                      { SDLK_LSHIFT,
                                                        "shift" } } };
  int index = unitialize_index;
  for (int i = 0; i < keys_count; i++) {
    if (keys[i].key == event.key.keysym.sym) {
      index = i;
      break;
    }
  }
  if (index != unitialize_index) {
    std::cout << keys[index].button << " ";
    if (event.type == SDL_KEYDOWN) {
      std::cout << "is pressed" << std::endl;
    } else {
      std::cout << "is released" << std::endl;
    }
  }
}

std::ostream&
operator<<(std::ostream& out, SDL_version& v)
{
  using namespace std;
  out << static_cast<int>(v.major) << '.' << static_cast<int>(v.minor) << '.'
      << static_cast<int>(v.patch) << endl;
  return out;
}

int
main(int /*argc*/, char** /*argv*/)
{
  using namespace std;
  SDL_version compiled;
  SDL_version linked;
  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    cerr << "failed to load sdl: " << SDL_GetError() << endl;
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("Game",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        800,
                                        600,
                                        SDL_WINDOW_OPENGL);
  if (window == nullptr) {
    std::cout << "failed to initialize window " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 2;
  }

  bool isTrue = true;
  while (isTrue) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        read_input(event);
      } else if (event.type == SDL_QUIT) {
        isTrue = false;
      }
    }
  }
  SDL_Quit();
  return 0;
}
