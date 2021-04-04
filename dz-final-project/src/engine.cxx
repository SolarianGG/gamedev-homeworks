#include "engine.hxx"
#include "../glad/include/glad/glad.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>

solar::engine::~engine() {}

// Vec2 and mat2x3 Implimintation

solar::vec2::vec2()
  : x(0.f)
  , y(0.f)
{}

solar::vec2::vec2(float x, float y)
  : x(x)
  , y(y)
{}

solar::vec2
operator+(const solar::vec2& l, const solar::vec2& r)
{
  solar::vec2 result;
  result.x = l.x + r.x;
  result.y = l.y + r.y;
  return result;
}

solar::mat2x3::mat2x3()
  : col0(1.0f, 0.0f)
  , col1(0.f, 1.f)
  , delta(0.f, 0.f)
{}

solar::mat2x3
solar::mat2x3::identity()
{
  return solar::mat2x3::scale(1.f);
}

solar::mat2x3
solar::mat2x3::scale(float scale)
{
  solar::mat2x3 result;
  result.col0.x = scale;
  result.col1.y = scale;
  return result;
}

solar::mat2x3
solar::mat2x3::scale(float sx, float sy)
{
  solar::mat2x3 r;
  r.col0.x = sx;
  r.col1.y = sy;
  return r;
}

solar::mat2x3
solar::mat2x3::rotation(float thetha)
{
  solar::mat2x3 result;

  result.col0.x = std::cos(thetha);
  result.col0.y = std::sin(thetha);

  result.col1.x = -std::sin(thetha);
  result.col1.y = std::cos(thetha);

  return result;
}

solar::mat2x3
solar::mat2x3::move(const solar::vec2& delta)
{
  solar::mat2x3 r = solar::mat2x3::identity();
  r.delta = delta;
  return r;
}

solar::vec2
operator*(const solar::vec2& v, const solar::mat2x3& m)
{
  solar::vec2 result;
  result.x = v.x * m.col0.x + v.y * m.col0.y + m.delta.x;
  result.y = v.x * m.col1.x + v.y * m.col1.y + m.delta.y;
  return result;
}

// -----------------------------------------------------

class engine_impl : public solar::engine
{
  SDL_Window* window = nullptr;
  SDL_GLContext context = nullptr;
  GLuint program = 0;
  uint32_t gl_default_vbo = 0;
  void set_uniform(const char* uniform_name, const solar::mat2x3& m);

public:
  bool init(int window_width, int window_height) override;
  bool read_input(solar::event& event) override;
  void render(const std::vector<solar::vertex>& vertex_buffer,
              const solar::mat2x3& m) override;
  void swap_buffers() override;
  void destroy() override;
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

// Init of engine_impl methods
bool
engine_impl::init(int window_width, int window_height)
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "Failed to init SDL (engine.cxx: 21): " << SDL_GetError()
              << std::endl;
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  window = SDL_CreateWindow("Game",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            window_width,
                            window_height,
                            SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    std::cerr << "Failed to create window (engine.cxx: 32): " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  context = SDL_GL_CreateContext(window);

  if (context == nullptr) {
    std::cerr << "Failed to create context (engine.cxx: 49): " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0) {
    std::cerr << "Failed to load GL functions (engine.cxx: 57)" << std::endl;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  glGenBuffers(1, &gl_default_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, gl_default_vbo);
  uint32_t data_size_in_bytes = 0;
  glBufferData(GL_ARRAY_BUFFER, data_size_in_bytes, nullptr, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_in_bytes, nullptr);

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

  /*  int location = glGetUniformLocation(program, "s_texture");

    int texture_unit = 0;
    glActiveTexture(GL_TEXTURE0 + texture_unit);

    if (!load_texture("tank.png")) {
      std::cerr << "error: Failed to load texture ( engine.cxx: 203 )"
                << std::endl;
      SDL_GL_DeleteContext(context);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return false;
    }

    // http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
    glUniform1i(location, 0 + texture_unit);
  */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  return true;
}

bool
engine_impl::read_input(solar::event& event)
{
  SDL_Event e;
  if (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      event.type = event.quit;
      return true;
    } else if (e.type == SDL_MOUSEMOTION) {
      event.type = event.mouse_motion;
      event.mouse_x = e.motion.x;
      event.mouse_y = e.motion.y;
      return true;
    } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
      event.type = (e.type == SDL_KEYDOWN) ? event.pressed : event.reliased;
      if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_UP) {
        event.key = event.top;
      } else if (e.key.keysym.sym == SDLK_s || e.key.keysym.sym == SDLK_DOWN) {
        event.key = event.bottom;
      } else if (e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_LEFT) {
        event.key = event.left;
      } else if (e.key.keysym.sym == SDLK_d || e.key.keysym.sym == SDLK_RIGHT) {
        event.key = event.right;
      }
      return true;
    }
  }
  return false;
}

void
engine_impl::set_uniform(const char* uniform_name, const solar::mat2x3& m)
{
  const int location = glGetUniformLocation(program, uniform_name);
  if (location == -1) {
    std::cerr << "can't get uniform location from shader\n";
    throw std::runtime_error("can't get uniform location");
  }
  // OpenGL wants matrix in column major order
  // clang-format off
        float values[9] = { m.col0.x,  m.col0.y, m.delta.x,
                            m.col1.x, m.col1.y, m.delta.y,
                            0.f,      0.f,       1.f };
  // clang-format on
  glUniformMatrix3fv(location, 1, GL_FALSE, &values[0]);
}

void
engine_impl::render(const std::vector<solar::vertex>& vertex_buffer,
                    const solar::mat2x3& m)
{
  glBindBuffer(GL_ARRAY_BUFFER, gl_default_vbo);
  const solar::vertex* t = vertex_buffer.data();
  uint32_t data_size_in_bytes =
    static_cast<uint32_t>(vertex_buffer.size() * sizeof(solar::vertex));
  glBufferData(GL_ARRAY_BUFFER, data_size_in_bytes, t, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_in_bytes, t);

  glEnableVertexAttribArray(0);
  GLintptr position_attr_offset = 0;
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(solar::vertex),
                        reinterpret_cast<void*>(position_attr_offset));

  glEnableVertexAttribArray(1);
  GLintptr color_attr_offset = sizeof(float) * 3;
  glVertexAttribPointer(1,
                        4,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(solar::vertex),
                        reinterpret_cast<void*>(color_attr_offset));
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

  glDrawArrays(GL_TRIANGLES, 0, 4);
}

void
engine_impl::swap_buffers()
{
  SDL_GL_SwapWindow(window);
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
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
solar::engine*
create_engine()
{
  engine_impl* e = new engine_impl();
  return e;
}

void
destroy_engine(solar::engine* e)
{
  if (e)
    delete e;
}
