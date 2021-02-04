#include <SDL2/SDL.h>

#include "engine.hxx"
#include <array>
#include <iostream>
#include <string_view>

int
main(int /*argc*/, char** /*argv*/)
{
  using namespace std;
  engine* e = create_engine();
  if (e == nullptr) {
    cerr << "failed to initialize engine" << std::endl;
    return 1;
  }

  bool is_loop = true;
  while (is_loop) {
    input in;
    while (e->read_input(in)) {
      if (in.event_type == ENGINE_QUIT_EVENT) {
        is_loop = false;
      } else if (in.key == "") {
        continue;
      } else {
        cout << in << endl;
      }
    }
  };
  destroy_engine(e);
  return 0;
}
