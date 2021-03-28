#include "engine.hxx"
#include "../glad/include/glad/glad.h"
#include "picopng.hxx"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <vector>

engine::~engine() {}

class engine_impl : public engine
{
  SDL_Window* window = nullptr;
  SDL_GLContext context = nullptr;
  GLuint program = 0;
  uint32_t gl_default_vbo = 0;

public:
  bool init() final;
  float get_time_from_init() final;
  bool read_input(event& e) final;
  bool load_texture(std::string_view path) final;
  void render_triangle(const std::vector<vertex>& t, glm::mat4x4 m) final;
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

  glGenBuffers(1, &gl_default_vbo);

  glBindBuffer(GL_ARRAY_BUFFER, gl_default_vbo);

  uint32_t data_size_in_bytes = 0;
  glBufferData(GL_ARRAY_BUFFER, data_size_in_bytes, nullptr, GL_STATIC_DRAW);

  glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_in_bytes, nullptr);

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  const char* vertex_shader_src = R"(
                                  #version 330 core
                                  layout (location = 0) in vec3 a_position;
                                  layout (location = 1) in vec2 a_tex_coord;
                                  uniform mat4x4 t;
                                  out vec2 v_tex_coord;

                                  void main()
                                  {
                                    v_tex_coord = a_tex_coord;
                                    gl_Position = t * vec4(a_position, 1.0);
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


                                    in vec2 v_tex_coord;
                                    uniform sampler2D s_texture;

                                    out vec4 frag_color;

                                    void main()
                                    {
                                        frag_color = texture(s_texture, v_tex_coord);
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
  glBindAttribLocation(program, 1, "a_tex_coord");

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

  int location = glGetUniformLocation(program, "s_texture");

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

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  return true;
}

float
engine_impl::get_time_from_init()
{
  uint32_t ticks = SDL_GetTicks();
  float seconds = ticks * 0.001f;
  return seconds;
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

bool
engine_impl::load_texture(std::string_view path)
{
  using namespace std;
  vector<byte> png_file_in_memory;
  ifstream texture(path.data(), ios::binary);
  if (!texture.is_open()) {
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
  GLuint tex_handl = 0;
  glGenTextures(1, &tex_handl);
  glBindTexture(GL_TEXTURE_2D, tex_handl);

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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  return true;
}

void
engine_impl::render_triangle(const std::vector<vertex>& t, glm::mat4 m)
{
  glBindBuffer(GL_ARRAY_BUFFER, gl_default_vbo);
  glBufferData(
    GL_ARRAY_BUFFER, sizeof(vertex) * t.size(), t.data(), GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex) * t.size(), t.data());
  int location = glGetUniformLocation(program, "t");

  glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(m));
  // glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));

  glEnableVertexAttribArray(0);
  GLintptr position_attr_offset = 0;
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(vertex),
                        reinterpret_cast<void*>(position_attr_offset));

  glEnableVertexAttribArray(1);
  GLintptr texture_attr_offset = sizeof(float) * 2;
  glVertexAttribPointer(1,
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
}

void
engine_impl::swap_buffers()
{
  SDL_GL_SwapWindow(window);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
