#include "engine.hxx"
#include "../glad/include/glad/glad.h"
#include "picopng.hxx"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <mutex>

vec2::vec2()
  : x(0.f)
  , y(0.f)
{}
vec2::vec2(float x_, float y_)
  : x(x_)
  , y(y_)
{}

vec2
operator+(const vec2& l, const vec2& r)
{
  vec2 result;
  result.x = l.x + r.x;
  result.y = l.y + r.y;
  return result;
}

mat2x3::mat2x3()
  : col0(1.0f, 0.f)
  , col1(0.f, 1.f)
  , delta(0.f, 0.f)
{}

mat2x3
mat2x3::identity()
{
  return mat2x3::scale(1.f);
}

mat2x3
mat2x3::scale(float scale)
{
  mat2x3 result;
  result.col0.x = scale;
  result.col1.y = scale;
  return result;
}

mat2x3
mat2x3::scale(float sx, float sy)
{
  mat2x3 r;
  r.col0.x = sx;
  r.col1.y = sy;
  return r;
}

mat2x3
mat2x3::rotation(float thetha)
{
  mat2x3 result;

  result.col0.x = std::cos(thetha);
  result.col0.y = std::sin(thetha);

  result.col1.x = -std::sin(thetha);
  result.col1.y = std::cos(thetha);

  return result;
}

mat2x3
mat2x3::move(const vec2& delta)
{
  mat2x3 r = mat2x3::identity();
  r.delta = delta;
  return r;
}

vec2
operator*(const vec2& v, const mat2x3& m)
{
  vec2 result;
  result.x = v.x * m.col0.x + v.y * m.col0.y + m.delta.x;
  result.y = v.x * m.col1.x + v.y * m.col1.y + m.delta.y;
  return result;
}

mat2x3
operator*(const mat2x3& m1, const mat2x3& m2)
{
  mat2x3 r;

  r.col0.x = m1.col0.x * m2.col0.x + m1.col1.x * m2.col0.y;
  r.col1.x = m1.col0.x * m2.col1.x + m1.col1.x * m2.col1.y;
  r.col0.y = m1.col0.y * m2.col0.x + m1.col1.y * m2.col0.y;
  r.col1.y = m1.col0.y * m2.col1.x + m1.col1.y * m2.col1.y;

  r.delta.x = m1.delta.x * m2.col0.x + m1.delta.y * m2.col0.y + m2.delta.x;
  r.delta.y = m1.delta.x * m2.col1.x + m1.delta.y * m2.col1.y + m2.delta.y;

  return r;
}

engine::~engine() {}

class engine_impl : public engine
{
  SDL_Window* window = nullptr;
  SDL_GLContext context = nullptr;
  GLuint program = 0;
  GLuint vertex_array_object = 0;
  std::vector<GLuint> texture_handles;

public:
  bool init() final;
  bool read_input(event& e) final;
  bool load_texture(std::string_view path, GLuint tex_handl) final;
  bool load_sound(std::string_view path, uint32_t sound_handl) final;
  void render(const triangle& t) final;
  void render(const std::vector<vertex>& vertex_buffer,
              const mat2x3& m,
              uint32_t texture_location) final;
  void set_uniforms(const uniform& un) final;
  void set_uniforms(const mat2x3& m);
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
                            800,
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
  glGenVertexArrays(1, &vertex_array_object);
  glBindVertexArray(vertex_array_object);

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  const char* vertex_shader_src = R"(
                                  #version 330 core
                                  layout (location = 0) in vec3 a_position;
                                  layout (location = 1) in vec4 a_color;
                                  layout (location = 2) in vec2 a_tex_coord;

                                  uniform mat3 u_matrix;

                                  out vec4 v_color;
                                  out vec2 v_tex_coord;
                                  void main()
                                  {
                                     v_color = a_color;
                                     v_tex_coord = a_tex_coord;
                                     vec3 pos = a_position * u_matrix;
                                     gl_Position = vec4(pos, 1.0);
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
                                    in vec2 v_tex_coord;

                                    uniform sampler2D s_texture;
                                    out vec4 frag_color;

                                    void main()
                                    {
                                        frag_color = texture(s_texture, v_tex_coord) * v_color;
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
  glBindAttribLocation(program, 2, "a_tex_coord");

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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return true;
}

bool
engine_impl::read_input(event& event)
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

    } else if (e.type == SDL_MOUSEBUTTONUP) {
      event.type = event.mouse_click;
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

bool
engine_impl::load_texture(std::string_view path, GLuint tex_handl)
{
  using namespace std;
  vector<byte> png_file_in_memory;
  ifstream texture(path.data(), ios::binary);
  if (!texture.is_open()) {
    std::cerr << "Failed to open: " << path << std::endl;
    return false;
  }
  texture.seekg(0, std::ios_base::end);
  size_t pos_in_file = static_cast<size_t>(texture.tellg());
  png_file_in_memory.resize(pos_in_file);
  texture.seekg(0, std::ios_base::beg);
  if (!texture) {
    return false;
  }
  texture.read(reinterpret_cast<char*>(png_file_in_memory.data()),
               static_cast<streamsize>(png_file_in_memory.size()));
  if (!texture.good()) {
    return false;
  }
  vector<byte> image;
  uint64_t w = 0;
  uint64_t h = 0;
  int error = decodePNG(
    image, w, h, &png_file_in_memory[0], png_file_in_memory.size(), false);
  if (error != 0) {
    cerr << "error: " << error << endl;
    return false;
  }
  texture_handles.push_back(0);
  glGenTextures(1, &texture_handles.at(tex_handl));
  glBindTexture(GL_TEXTURE_2D, texture_handles.at(tex_handl));

  GLint mipmap_level = 0;
  GLint border = 0;

  glTexImage2D(GL_TEXTURE_2D,
               mipmap_level,
               GL_RGBA,
               static_cast<GLsizei>(w),
               static_cast<GLsizei>(h),
               border,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               &image[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  return true;
}

bool
engine_impl::load_sound(std::string_view path, uint32_t sound_handl)
{

  return true;
}

void
engine_impl::render(const triangle& t)
{
  glBufferData(GL_ARRAY_BUFFER, sizeof(t), &t, GL_DYNAMIC_DRAW);

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

  glEnableVertexAttribArray(2);
  GLintptr texture_attr_offset = sizeof(float) * 7;
  glVertexAttribPointer(2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(vertex),
                        reinterpret_cast<void*>(texture_attr_offset));

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
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

void
engine_impl::render(const std::vector<vertex>& vertex_buffer,
                    const mat2x3& m,
                    uint32_t texture_location)
{
  set_uniforms(m);
  int location = glGetUniformLocation(program, "s_texture");
  glActiveTexture(GL_TEXTURE0 + texture_location);
  glBindTexture(GL_TEXTURE_2D, texture_handles.at(texture_location));
  glUniform1i(location, 0 + texture_location);

  glBufferData(GL_ARRAY_BUFFER,
               vertex_buffer.size() * sizeof(vertex),
               vertex_buffer.data(),
               GL_DYNAMIC_DRAW);

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
  glEnableVertexAttribArray(2);
  GLintptr texture_attr_offset = sizeof(float) * 7;
  glVertexAttribPointer(2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(vertex),
                        reinterpret_cast<void*>(texture_attr_offset));
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
  glDrawArrays(GL_TRIANGLES, 0, vertex_buffer.size());
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

void
engine_impl::set_uniforms(const mat2x3& m)
{
  const int location = glGetUniformLocation(program, "u_matrix");
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
engine_impl::set_uniforms(const uniform& un)
{
  float f0;
  float f1;
  f0 = static_cast<float>(un.f0) / static_cast<float>(799) * 2 - 1.0f;
  f1 = -(static_cast<float>(un.f1) / static_cast<float>(599)) * 2 + 1.0f;
  int location = glGetUniformLocation(program, "mouse_position");
  glUniform2f(location, f0, f1);
}

void
engine_impl::swap_buffers()
{
  SDL_GL_SwapWindow(window);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
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
