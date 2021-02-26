#include "engine.hxx"

#include "../glad/include/KHR/khrplatform.h"
#include "../glad/include/glad/glad.h"
#include <SDL2/SDL.h>
#include <array>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <thread>

static void APIENTRY
callback_opengl_debug(GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar* message,
                      [[maybe_unused]] const void* userParam);

// Keyboard keys and variables

constexpr std::string_view engine_quit_event = "engine_quit";
constexpr std::string_view engine_keyboard_pressed_event = "pressed";
constexpr std::string_view engine_keyboard_released_event = "released";

constexpr int unitialize_index = -1;
constexpr int keys_count = 8;

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
    void* delete_game_func_ptr = SDL_LoadFunction(old_handle, "delete_game");
    if (delete_game_func_ptr == nullptr) {
      std::cerr << "error: no function [delete_game] in " << library_name << " "
                << SDL_GetError() << std::endl;
      return nullptr;
    }
    // We use decltype to find out what type return' our function
    typedef decltype(&delete_game) delete_game_ptr;

    // We use reinterpret_cast for hard polymorph of return value of our
    // function
    auto delete_game_func =
      reinterpret_cast<delete_game_ptr>(delete_game_func_ptr);

    delete_game_func(old);
    SDL_UnloadObject(old_handle);
  }

  if (std::filesystem::exists(tmp_library_name)) {
    if (0 == remove(tmp_library_name)) {
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
  // We use decltype to find out what type return' our function
  typedef decltype(&create_game) create_game_ptr;

  // We use reinterpret_cast for hard polymorph of return value of our function
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

  // compares compiled and dynamic linked SDL lib
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

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  // Creating simple window for reading input
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
  int gl_major_ver = 3;
  int gl_minor_ver = 2;
  int gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    gl_major_ver = 3;
    gl_minor_ver = 2;
    gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);
    gl_context = SDL_GL_CreateContext(window);
  }

  int result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
  assert(result == 0);

  result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
  assert(result == 0);

  if (gl_major_ver < 3) {
    std::clog << "current context opengl version: " << gl_major_ver << '.'
              << gl_minor_ver << '\n'
              << "need openg ES version at least: 3.2\n"
              << std::flush;
    throw std::runtime_error("opengl version too low");
  }

  std::clog << "OpenGl " << gl_major_ver << '.' << gl_minor_ver << '\n';

  if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0) {
    std::clog << "error: failed to initialize glad" << std::endl;
  }

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(callback_opengl_debug, nullptr);
  glDebugMessageControl(
    GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

  // In this code we specially lose our window to facilitate our engine impl
  return new engine(window);
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

engine::engine(SDL_Window* window)
  : window(window)
{}

bool
engine::read_input(input& e)
{
  using namespace std;
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    e.key = check_key(event);
    if (event.type == SDL_KEYDOWN) {
      e.event_type = engine_keyboard_pressed_event;
    } else if (event.type == SDL_KEYUP) {
      e.event_type = engine_keyboard_released_event;
    } else if (event.type == SDL_QUIT) {
      e.event_type = engine_quit_event;
    }
    return true;
  }
  return false;
}

void
engine::render_triangle()
{
  using namespace std;
  chrono::steady_clock clock;
  auto time_point = clock.now();
  auto time_since_epoch = time_point.time_since_epoch();
  auto ns = time_since_epoch.count();
  auto seconds = ns / 1000'000'000.0f;
  auto current_color = 0.5f * (std::sin(seconds) + 1.0f);

  glClearColor(0.f, current_color, 1.f - current_color, 0.0f);

  glClear(GL_COLOR_BUFFER_BIT);
}

void
engine::swap_window()
{
  SDL_GL_SwapWindow(window);
}

engine::~engine() {}

// ----------------------------------------------------------------------

static const char*
source_to_strv(GLenum source)
{
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      return "API";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "SHADER_COMPILER";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
      return "OTHER";
  }
  return "unknown";
}

static const char*
type_to_strv(GLenum type)

{
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "PERFORMANCE";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "PORTABILITY";
    case GL_DEBUG_TYPE_MARKER:
      return "MARKER";
    case GL_DEBUG_TYPE_PUSH_GROUP:
      return "PUSH_GROUP";
    case GL_DEBUG_TYPE_POP_GROUP:
      return "POP_GROUP";
    case GL_DEBUG_TYPE_OTHER:
      return "OTHER";
  }
  return "unknown";
}

static const char*
severity_to_strv(GLenum severity)
{
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      return "HIGH";
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "MEDIUM";
    case GL_DEBUG_SEVERITY_LOW:
      return "LOW";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "NOTIFICATION";
  }
  return "unknown";
}

static std::array<char, GL_MAX_DEBUG_MESSAGE_LENGTH> local_log_buff;

static void APIENTRY
callback_opengl_debug(GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar* message,
                      [[maybe_unused]] const void* userParam)
{
  // The memory formessageis owned and managed by the GL, and should onlybe
  // considered valid for the duration of the function call.The behavior of
  // calling any GL or window system function from within thecallback function
  // is undefined and may lead to program termination.Care must also be taken
  // in securing debug callbacks for use with asynchronousdebug output by
  // multi-threaded GL implementations.  Section 18.8 describes thisin further
  // detail.

  auto& buff{ local_log_buff };
  int num_chars = std::snprintf(buff.data(),
                                buff.size(),
                                "%s %s %d %s %.*s\n",
                                source_to_strv(source),
                                type_to_strv(type),
                                id,
                                severity_to_strv(severity),
                                length,
                                message);

  if (num_chars > 0) {
    // TODO use https://en.cppreference.com/w/cpp/io/basic_osyncstream
    // to fix possible data races
    // now we use GL_DEBUG_OUTPUT_SYNCHRONOUS to garantie call in main
    // thread
    std::cerr.write(buff.data(), num_chars);
  }
}

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
      if (in.event_type == engine_quit_event) {
        is_loop = false;
      } else if (in.key == "") {
        continue;
      } else if (in.event_type == engine_keyboard_pressed_event ||
                 in.event_type == engine_keyboard_released_event) {
        g->on_event(in);
      }
    }
    e->render_triangle();
    e->swap_window();
  }

  return 0;
}
