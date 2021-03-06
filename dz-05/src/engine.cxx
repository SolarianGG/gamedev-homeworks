#include "engine.hxx"
#include "../glad/include/glad/glad.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

engine::~engine() {}

class engine_impl : public engine
{
  SDL_Window* window = nullptr;
  SDL_GLContext context = nullptr;
  GLuint program = 0;

public:
  bool init() final;
  bool read_input(event& e) final;
  void render_triangle(const triangle& t) final;
  void swap_buffers() final;
  void destroy() final;
};

static bool
init_shader(GLuint shader, const char* shader_src)
{
  glShaderSource(shader, 1, &shader_src, nullptr);
  glCompileShader(shader);

  GLint compile_status = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
  if (compile_status == 0) {
    GLint info_len = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
    std::vector<char> info(static_cast<size_t>(info_len));
    glGetShaderInfoLog(shader, info_len, nullptr, info.data());
    glDeleteShader(shader);
    return false;
  }
  return true;
}

bool
engine_impl::init()
{
  SDL_version compiled;
  SDL_version linked;
  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);
  if (SDL_COMPILEDVERSION !=
      SDL_VERSIONNUM(linked.major, linked.minor, linked.patch)) {
    std::cerr << "error: There are different versions of compiled and linked "
                 "SDL ( engine.cxx: 51 )"
              << std::endl;
    return false;
  }

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "error: Failed to init SDL ( engine.cxx: 57 )"
              << SDL_GetError() << std::endl;
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  window = SDL_CreateWindow("Game",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            800,
                            600,
                            SDL_WINDOW_OPENGL);
  if (window == nullptr) {
    std::cerr << "error: Failed to create window ( engine.cxx: 70 )"
              << SDL_GetError() << std::endl;
    SDL_Quit();
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  context = SDL_GL_CreateContext(window);
  if (context == nullptr) {
    std::cerr << "error: Failed to create context ( engine.cxx: 82 )"
              << SDL_GetError() << std::endl;

    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0) {
    std::cerr << "error: Failed to load GL functions ( engine.cxx: 91 )"
              << std::endl;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  GLuint vertex_buffer = 0;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  GLuint vertex_array_object = 0;
  glGenVertexArrays(1, &vertex_array_object);
  glBindVertexArray(vertex_array_object);

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  const char* vertex_shader_src = R"(
                                  #version 330 core
                                  layout (location = 0) in vec3 a_position;
                                  layout (location = 1) in vec4 a_color;

                                  out vec4 v_color;

                                  void main()
                                  {
                                    v_color = a_color;
                                    gl_Position = vec4(a_position, 1.0);
                                  }
                                  )";
  if (!init_shader(vertex_shader, vertex_shader_src)) {
    std::cerr << "error: Failed to create vertex shader ( engine.cxx: 121 )"
              << std::endl;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  const char* fragment_shader_src = R"(
                                    #version 330 core


                                    in vec4 v_color;

                                    out vec4 frag_color;

                                    void main()
                                    {
                                        frag_color = v_color;
                                    }
                                    )";
  if (!init_shader(fragment_shader, fragment_shader_src)) {
    std::cerr << "error: Failed to create fragment shader ( engine.cxx: 143 )"
              << std::endl;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  program = glCreateProgram();
  if (program == 0) {
    std::cerr << "error: Failed to create program ( engine.cxx: 152 )"
              << std::endl;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);

  glBindAttribLocation(program, 0, "a_position");
  glBindAttribLocation(program, 1, "a_color");

  glLinkProgram(program);

  GLint linked_status = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &linked_status);

  if (linked_status == 0) {
    GLint infoLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
    std::vector<char> infoLog(static_cast<size_t>(infoLen));
    glGetProgramInfoLog(program, infoLen, nullptr, infoLog.data());
    glDeleteProgram(program);
    std::cerr << "error: Failed to link program ( engine.cxx: 183 )"
              << std::endl;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  glUseProgram(program);

  glEnable(GL_DEPTH_TEST);

  return true;
}

bool
engine_impl::read_input(event& e)
{
  using namespace std;
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    if ((event.key.keysym.sym >= '0' && event.key.keysym.sym <= 'z') &&
        (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)) {
      e.key = event.key.keysym.sym;
      if (event.type == SDL_KEYDOWN) {
        e.event_type = event_pressed;
      } else if (event.type == SDL_KEYUP) {
        e.event_type = event_reliased;
      } else if (event.type == SDL_QUIT) {
        e.event_type = event_quit;
      }
      return true;
    } else {
      if (event.type == SDL_QUIT) {
        e.event_type = event_quit;
        return true;
      }
      return false;
    }
  }
  return false;
}

void
engine_impl::render_triangle(const triangle& t)
{
  glBufferData(GL_ARRAY_BUFFER, sizeof(t), &t, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  GLintptr position_attr_offset = 0;
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(vertex),
                        reinterpret_cast<void*>(position_attr_offset));

  glEnableVertexAttribArray(1);
  GLintptr color_attr_offset = sizeof(float) * 3;
  glVertexAttribPointer(1,
                        4,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(vertex),
                        reinterpret_cast<void*>(color_attr_offset));
  glValidateProgram(program);

  // Check the validate status
  GLint validate_status = 0;
  glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_status);

  if (validate_status == GL_FALSE) {
    GLint infoLen = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);

    std::vector<char> infoLog(static_cast<size_t>(infoLen));
    glGetProgramInfoLog(program, infoLen, nullptr, infoLog.data());
    std::cerr << "Error linking program:\n" << infoLog.data();
    throw std::runtime_error("error");
  }
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void
engine_impl::swap_buffers()
{
  SDL_GL_SwapWindow(window);

  glClearColor(0.3f, 0.3f, 1.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
engine_impl::destroy()
{
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

// ---------------------------------------

engine*
create_engine()
{
  engine_impl* engine = new engine_impl();
  return engine;
}

void
destroy_engine(engine* e)
{
  if (e)
    delete e;
}
