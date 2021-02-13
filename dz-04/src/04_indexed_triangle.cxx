#include "04_indexed_triangle.hxx"
#include <vector>

static int
exists(std::vector<position>& arr, position value)
{
  using namespace std;
  for (size_t i = 0; i < arr.size(); i++) {
    if (arr.at(i) == value) {
      return i;
    }
  }
  return -1;
}

void
indexed_triangle_render::add_triangle(position v0, position v1, position v2)
{
  using namespace std;
  vector<position>& vertex_buffer = get_vertex_buffer();
  int index;

  if ((index = exists(vertex_buffer, v0)) == -1) {
    vertex_buffer.push_back(v0);
    index = exists(vertex_buffer, v0);
  }
  index_buffer.push_back(index);

  if ((index = exists(vertex_buffer, v1)) == -1) {
    vertex_buffer.push_back(v1);
    index = exists(vertex_buffer, v1);
  }
  index_buffer.push_back(index);

  if ((index = exists(vertex_buffer, v2)) == -1) {
    vertex_buffer.push_back(v2);
    index = exists(vertex_buffer, v2);
  }
  index_buffer.push_back(index);
}

void
indexed_triangle_render::draw_triangles(const color& rgb)
{
  for (size_t i = 0; i < index_buffer.size() / 3; i++) {
    auto i0 = index_buffer.at(3 * i + 0);
    auto i1 = index_buffer.at(3 * i + 1);
    auto i2 = index_buffer.at(3 * i + 2);
    auto& vert_buff = get_vertex_buffer();
    draw_triangle(vert_buff.at(i0), vert_buff.at(i1), vert_buff.at(i2), rgb);
  }
}

void
indexed_triangle_render::clear_buffer()
{
  vertex_buffer_triangle_render::clear_buffer();
  index_buffer.clear();
}
