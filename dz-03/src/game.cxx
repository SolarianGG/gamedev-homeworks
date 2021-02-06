#include "engine.hxx"
#include <iostream>

class test_game : public game
{
  engine& e; // We can easily remove this field but i don't want to do it

public:
  explicit test_game(engine& e)
    : e(e)
  {}
  void on_event(input& in) override
  {
    using namespace std;
    cout << in << endl;
  }
  ~test_game() {}
};

game*
create_game(engine* e)
{
  if (e == nullptr) {
    return nullptr;
  }
  return new test_game(*e);
}

void
delete_game(game* g)
{
  if (g == nullptr) {
    return;
  }
  delete g;
}
