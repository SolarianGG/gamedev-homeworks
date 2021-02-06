#include "engine.hxx"

#include <SDL2/SDL.h>
#include <array>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <thread>

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

static game*
reload_game(game* old,
            const char* library_name,
            const char* tmp_library_name,
            engine& engine,
            void*& old_handle)
{
  using namespace std::filesystem;

  if (old != nullptr) {
    void* destroy_game_func_ptr = SDL_LoadFunction(old_handle, "delete_game");
    if (destroy_game_func_ptr == nullptr) {
      std::cerr << "error: no function [delete_game] in " << library_name << " "
                << SDL_GetError() << std::endl;
      return nullptr;
    }
    typedef decltype(&delete_game) delete_game_ptr;

    auto delete_game_func =
      reinterpret_cast<delete_game_ptr>(destroy_game_func_ptr);

    delete_game_func(old);
    SDL_UnloadObject(old_handle);
  }

  if (std::filesystem::exists(tmp_library_name)) {
    if (0 != remove(tmp_library_name)) {
      std::cerr << "error: can't remove: " << tmp_library_name << std::endl;
      return nullptr;
    }
  }

  try {
    copy(library_name, tmp_library_name); // throw on error
  } catch (const std::exception& ex) {
    std::cerr << "error: can't copy [" << library_name << "] to ["
              << tmp_library_name << "]" << std::endl;
    return nullptr;
  }

  void* game_handle = SDL_LoadObject(library_name);

  if (game_handle == nullptr) {
    std::cerr << "error: failed to load: [" << library_name << "] "
              << SDL_GetError() << std::endl;
    return nullptr;
  }

  old_handle = game_handle;

  void* create_game_func_ptr = SDL_LoadFunction(game_handle, "create_game");

  if (create_game_func_ptr == nullptr) {
    std::cerr << "error: no function [create_game] in " << library_name << " "
              << SDL_GetError() << std::endl;
    return nullptr;
  }

  typedef decltype(&create_game) create_game_ptr;

  auto create_game_func =
    reinterpret_cast<create_game_ptr>(create_game_func_ptr);

  game* game = create_game_func(&engine);

  if (game == nullptr) {
    std::cerr << "error: func [create_game] returned: nullptr" << std::endl;
    return nullptr;
  }
  return game;
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

int
main(int /*argc*/, char** /*argv*/)
{
  using namespace std;
  unique_ptr<engine, void (*)(engine*)> e(create_engine(), destroy_engine);
  if (e == nullptr) {
    cerr << "failed to initialize engine" << std::endl;
    return 1;
  }

  const char* libenginename = "./libgame.so";

  const char* tmp_lib = "./temp.so";

  void* handle = {};

  game* g = reload_game(nullptr, libenginename, tmp_lib, *e, handle);
  if (g == nullptr) {
    cerr << "failed to initialize game" << std::endl;
    return 2;
  }

  auto loading_time = std::filesystem::last_write_time(libenginename);

  bool is_loop = true;
  while (is_loop) {
    using namespace std::filesystem;
    if (exists(libenginename)) {
      auto current_loading_time = last_write_time(libenginename);

      if (loading_time != current_loading_time) {
        file_time_type next_loading_time;
        for (;;) {
          using namespace std::chrono;
          std::this_thread::sleep_for(milliseconds(100));
          next_loading_time = std::filesystem::last_write_time(libenginename);
          if (next_loading_time != current_loading_time) {
            current_loading_time = next_loading_time;
          } else {
            break;
          }
        }

        std::cout << "reloading game" << std::endl;
        g = reload_game(g, libenginename, tmp_lib, *e, handle);

        if (g == nullptr) {
          std::cerr << "next attemp to reload game..." << std::endl;
          continue;
        }

        loading_time = next_loading_time;
      }
    }

    input in;
    while (e->read_input(in)) {
      if (in.event_type == ENGINE_QUIT_EVENT) {
        is_loop = false;
      } else if (in.key == "") {
        continue;
      } else {
        g->on_event(in);
      }
    }
  }

  return 0;
}
