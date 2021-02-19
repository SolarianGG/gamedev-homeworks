#include "00_basic_canvas.hxx"
#include "05_interpolated_triangle.hxx"

#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>
#include <thread>

struct gfx_program : public shader_program
{
  uniforms uniform;

public:
  void set_uniforms(const uniforms& un_in) override { uniform = un_in; }

  vertex vertex_shader(const vertex& v_in) override
  {
    vertex ver = v_in;
    int mouse_x = uniform.f0;
    int mouse_y = uniform.f1;
    int left_x = uniform.f2;
    int right_x = uniform.f3;
    int top_y = uniform.f4;
    int bot_y = uniform.f5;
    if (mouse_x >= left_x && mouse_x <= right_x && mouse_y >= top_y &&
        mouse_y <= bot_y) {
      ver.col = blue;
    }
    return ver;
  }

  color fragment_shader(const vertex& v_in) override
  {
    color c = v_in.col;
    /* int mouse_x = uniform.f0;
     int mouse_y = uniform.f1;
     int radius = uniform.f2;
     int dx = mouse_x - v_in.pos.x;
     int dy = mouse_y - v_in.pos.y;
     if (dx * dx + dy * dy < radius * radius) {
       double gray = 0.21 * c.r + 0.72 * c.g + 0.07 * c.b;
       c.r = gray;
       c.g = gray;
       c.b = gray;
     }*/
    return c;
  }
};

int
main(int /*argc*/, char** /*argv*/)
{
  interpolated_triangle_render interpolated_renderer(new image(320, 240));
  if (0 != SDL_Init(SDL_INIT_EVERYTHING)) {
    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
    return 1;
  }

  auto width = interpolated_renderer.get_img().get_width();
  auto height = interpolated_renderer.get_img().get_height();

  SDL_Window* window = SDL_CreateWindow("MyOwnShader",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        width,
                                        height,
                                        SDL_WINDOW_OPENGL);
  if (window == nullptr) {
    std::cerr << "Failed to initialize window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 2;
  }

  SDL_Renderer* render =
    SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (render == nullptr) {
    std::cerr << "Failed to initialize renderer: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 3;
  }

  interpolated_renderer.add_triangle(vertex{ position{ 150, 30 }, red },
                                     vertex{ position{ 125, 50 }, red },
                                     vertex{ position{ 175, 50 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 150, 30 }, red },
                                     vertex{ position{ 125, 50 }, red },
                                     vertex{ position{ 100, 10 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 150, 30 }, red },
                                     vertex{ position{ 200, 10 }, red },
                                     vertex{ position{ 175, 50 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 60, 40 }, red },
                                     vertex{ position{ 125, 50 }, red },
                                     vertex{ position{ 100, 10 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 240, 40 }, red },
                                     vertex{ position{ 200, 10 }, red },
                                     vertex{ position{ 175, 50 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 150, 180 }, red },
                                     vertex{ position{ 125, 50 }, red },
                                     vertex{ position{ 175, 50 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 150, 180 }, red },
                                     vertex{ position{ 125, 50 }, red },
                                     vertex{ position{ 30, 120 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 125, 50 }, red },
                                     vertex{ position{ 60, 40 }, red },
                                     vertex{ position{ 30, 120 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 150, 180 }, red },
                                     vertex{ position{ 75, 160 }, red },
                                     vertex{ position{ 30, 120 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 150, 180 }, red },
                                     vertex{ position{ 270, 120 }, red },
                                     vertex{ position{ 175, 50 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 240, 40 }, red },
                                     vertex{ position{ 270, 120 }, red },
                                     vertex{ position{ 175, 50 }, red });
  interpolated_renderer.add_triangle(vertex{ position{ 220, 160 }, red },
                                     vertex{ position{ 270, 120 }, red },
                                     vertex{ position{ 150, 180 }, red });

  int left_x = 30;
  int right_x = 270;
  int top_y = 10;
  int bot_y = 180;
  int mouse_x = 0;
  int mouse_y = 0;
  uniforms un = { mouse_x, mouse_y, left_x, right_x, top_y, bot_y };

  gfx_program* gfx = new gfx_program();
  gfx->set_uniforms(un);

  interpolated_renderer.set_gfx_program(gfx);

  void* data = interpolated_renderer.get_img().get_pixels().data();
  const int depth = sizeof(color) * 8;
  const int pitch = width * sizeof(color);
  const uint32_t rmask = 0x000000ff;
  const uint32_t gmask = 0x0000ff00;
  const uint32_t bmask = 0x00ff0000;

  bool is_loop = true;
  while (is_loop) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        is_loop = false;
      } else if (event.type == SDL_MOUSEMOTION) {
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
      }
    }

    interpolated_renderer.clear(black);

    gfx->set_uniforms(
      uniforms{ mouse_x, mouse_y, left_x, right_x, top_y, bot_y });

    interpolated_renderer.draw_triangles();

    SDL_Surface* bitmup_surface = SDL_CreateRGBSurfaceFrom(
      data, width, height, depth, pitch, rmask, gmask, bmask, 0);

    if (bitmup_surface == nullptr) {
      std::cerr << "Failed to create surface: " << SDL_GetError() << std::endl;
      SDL_DestroyRenderer(render);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 4;
    }
    SDL_Texture* bitmup_texture =
      SDL_CreateTextureFromSurface(render, bitmup_surface);

    if (bitmup_texture == nullptr) {
      std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
      SDL_DestroyRenderer(render);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 5;
    }
    SDL_FreeSurface(bitmup_surface);

    SDL_RenderClear(render);
    SDL_RenderCopy(render, bitmup_texture, nullptr, nullptr);
    SDL_RenderPresent(render);
    SDL_DestroyTexture(bitmup_texture);
  }

  SDL_DestroyRenderer(render);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
