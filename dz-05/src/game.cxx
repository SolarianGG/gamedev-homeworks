#include "engine.hxx"
#include <cmath>
#include <iostream>
#include <vector>

struct cell
{
  vertex v0;
  vertex v1;
  vertex v2;
  vertex v3;
};

void
add_to_buffer(std::vector<vertex>& v,
              vertex& v0,
              vertex& v1,
              vertex& v2,
              vertex& v3)
{
  v.push_back(v0);
  v.push_back(v1);
  v.push_back(v2);
  v.push_back(v1);
  v.push_back(v2);
  v.push_back(v3);
}

void
find_way(int field[8][8], int ix, int iy, int out[8][8])
{
  out[iy][ix] = 3;
  // white pawn
  if (field[iy][ix] == 1) {
    if (field[iy + 1][ix - 1] < 0 && ix - 1 >= 0 && iy + 1 < 8) {
      out[iy + 1][ix - 1] = 2;
    }
    if (field[iy + 1][ix + 1] < 0 && ix + 1 != 8 && iy + 1 < 8) {
      out[iy + 1][ix + 1] = 2;
    }
    if (field[iy + 1][ix] == 0 && iy + 1 < 8) {
      out[iy + 1][ix] = 1;
    }
    if (field[iy + 2][ix] == 0 && iy == 1 && field[iy + 1][ix] == 0) {
      out[iy + 2][ix] = 1;
    }
  }
  // white horse
  else if (field[iy][ix] == 2) {
    // top
    if (field[iy + 1][ix + 2] < 0 && ix + 2 < 8 && iy + 1 < 8) {
      out[iy + 1][ix + 2] = 2;
    }
    if (field[iy + 1][ix + 2] == 0 && ix + 2 < 8 && iy + 1 < 8) {
      out[iy + 1][ix + 2] = 1;
    }
    if (field[iy + 2][ix + 1] < 0 && ix + 1 < 8 && iy + 2 < 8) {
      out[iy + 2][ix + 1] = 2;
    }
    if (field[iy + 2][ix + 1] == 0 && ix + 1 < 8 && iy + 2 < 8) {
      out[iy + 2][ix + 1] = 1;
    }
    if (field[iy + 1][ix - 2] < 0 && ix - 2 >= 0 && iy + 1 < 8) {
      out[iy + 1][ix - 2] = 2;
    }
    if (field[iy + 1][ix - 2] == 0 && ix - 2 >= 0 && iy + 1 < 8) {
      out[iy + 1][ix - 2] = 1;
    }
    if (field[iy + 2][ix - 1] < 0 && ix - 1 >= 0 && iy + 2 < 8) {
      out[iy + 2][ix - 1] = 2;
    }
    if (field[iy + 2][ix - 1] == 0 && ix - 1 >= 0 && iy + 2 < 8) {
      out[iy + 2][ix - 1] = 1;
    }
    // bot
    if (field[iy - 1][ix + 2] < 0 && ix + 2 < 8 && iy - 1 >= 0) {
      out[iy - 1][ix + 2] = 2;
    }
    if (field[iy - 1][ix + 2] == 0 && ix + 2 < 8 && iy - 1 >= 0) {
      out[iy - 1][ix + 2] = 1;
    }
    if (field[iy - 2][ix + 1] < 0 && ix + 1 < 8 && iy - 2 >= 0) {
      out[iy - 2][ix + 1] = 2;
    }
    if (field[iy - 2][ix + 1] == 0 && ix + 1 < 8 && iy - 2 >= 0) {
      out[iy - 2][ix + 1] = 1;
    }
    if (field[iy - 1][ix - 2] < 0 && ix - 2 >= 0 && iy - 1 >= 0) {
      out[iy - 1][ix - 2] = 2;
    }
    if (field[iy - 1][ix - 2] == 0 && ix - 2 >= 0 && iy - 1 >= 0) {
      out[iy - 1][ix - 2] = 1;
    }
    if (field[iy - 2][ix - 1] < 0 && ix - 1 >= 0 && iy - 2 >= 0) {
      out[iy - 2][ix - 1] = 2;
    }
    if (field[iy - 2][ix - 1] == 0 && ix - 1 >= 0 && iy - 2 >= 0) {
      out[iy - 2][ix - 1] = 1;
    }
  }
  // white slon
  else if (field[iy][ix] == 3) {
    int i;
    int j;
    // top
    for (i = iy + 1, j = ix + 1; i < 8 || j < 8; i++, j++) {
      if (field[i][j] < 0 && i < 8 && j < 8) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i < 8 && j < 8) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    for (i = iy + 1, j = ix - 1; i < 8 || j >= 0; i++, j--) {
      if (field[i][j] < 0 && i < 8 && j >= 0) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i < 8 && j >= 0) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    // bot
    for (i = iy - 1, j = ix + 1; i >= 0 || j < 8; i--, j++) {
      if (field[i][j] < 0 && i >= 0 && j < 8) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i >= 0 && j < 8) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    for (i = iy - 1, j = ix - 1; i >= 0 || j >= 0; i--, j--) {
      if (field[i][j] < 0 && i >= 0 && j >= 0) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i >= 0 && j >= 0) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
  } // white rook
  else if (field[iy][ix] == 4) {
    // top and bot
    for (int i = iy + 1; i < 8; i++) {
      if (field[i][ix] < 0 && i < 8) {
        out[i][ix] = 2;
      }
      if (field[i][ix] == 0 && i < 8) {
        out[i][ix] = 1;
      } else {
        break;
      }
    }
    for (int i = iy - 1; i >= 0; i--) {
      if (field[i][ix] < 0 && i >= 0) {
        out[i][ix] = 2;
      }
      if (field[i][ix] == 0 && i >= 0) {
        out[i][ix] = 1;
      } else {
        break;
      }
    }
    // left and right
    for (int j = ix + 1; j < 8; j++) {
      if (field[iy][j] < 0 && j < 8) {
        out[iy][j] = 2;
      }
      if (field[iy][j] == 0 && j < 8) {
        out[iy][j] = 1;
      } else {
        break;
      }
    }
    for (int j = ix - 1; j >= 0; j--) {
      if (field[iy][j] < 0 && j >= 0) {
        out[iy][j] = 2;
      }
      if (field[iy][j] == 0 && j >= 0) {
        out[iy][j] = 1;
      } else {
        break;
      }
    }
  }
  // white queen
  else if (field[iy][ix] == 5) {
    // top and bot
    for (int i = iy + 1; i < 8; i++) {
      if (field[i][ix] < 0 && i < 8) {
        out[i][ix] = 2;
      }
      if (field[i][ix] == 0 && i < 8) {
        out[i][ix] = 1;
      } else {
        break;
      }
    }
    for (int i = iy - 1; i >= 0; i--) {
      if (field[i][ix] < 0 && i >= 0) {
        out[i][ix] = 2;
      }
      if (field[i][ix] == 0 && i >= 0) {
        out[i][ix] = 1;
      } else {
        break;
      }
    }
    // left and right
    for (int j = ix + 1; j < 8; j++) {
      if (field[iy][j] < 0 && j < 8) {
        out[iy][j] = 2;
      }
      if (field[iy][j] == 0 && j < 8) {
        out[iy][j] = 1;
      } else {
        break;
      }
    }
    for (int j = ix - 1; j >= 0; j--) {
      if (field[iy][j] < 0 && j >= 0) {
        out[iy][j] = 2;
      }
      if (field[iy][j] == 0 && j >= 0) {
        out[iy][j] = 1;
      } else {
        break;
      }
    }
    int i;
    int j;
    // top
    for (i = iy + 1, j = ix + 1; i < 8 || j < 8; i++, j++) {
      if (field[i][j] < 0 && i < 8 && j < 8) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i < 8 && j < 8) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    for (i = iy + 1, j = ix - 1; i < 8 || j >= 0; i++, j--) {
      if (field[i][j] < 0 && i < 8 && j >= 0) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i < 8 && j >= 0) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    // bot
    for (i = iy - 1, j = ix + 1; i >= 0 || j < 8; i--, j++) {
      if (field[i][j] < 0 && i >= 0 && j < 8) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i >= 0 && j < 8) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    for (i = iy - 1, j = ix - 1; i >= 0 || j >= 0; i--, j--) {
      if (field[i][j] < 0 && i >= 0 && j >= 0) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i >= 0 && j >= 0) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
  }
  // white king
  else if (field[iy][ix] == 6) {
    if (field[iy][ix - 1] < 0 && ix - 1 >= 0) {
      out[iy][ix - 1] = 2;
    }
    if (field[iy][ix - 1] == 0 && ix - 1 >= 0) {
      out[iy][ix - 1] = 1;
    }
    if (field[iy][ix + 1] < 0 && ix + 1 < 8) {
      out[iy][ix + 1] = 2;
    }
    if (field[iy][ix + 1] == 0 && ix + 1 < 8) {
      out[iy][ix + 1] = 1;
    }
    if (field[iy - 1][ix - 1] < 0 && ix - 1 >= 0 && iy - 1 >= 0) {
      out[iy - 1][ix - 1] = 2;
    }
    if (field[iy - 1][ix - 1] == 0 && ix - 1 >= 0 && iy - 1 >= 0) {
      out[iy - 1][ix - 1] = 1;
    }
    if (field[iy - 1][ix + 1] < 0 && ix + 1 < 8 && iy - 1 >= 0) {
      out[iy - 1][ix + 1] = 2;
    }
    if (field[iy - 1][ix + 1] == 0 && ix + 1 < 8 && iy - 1 >= 0) {
      out[iy - 1][ix + 1] = 1;
    }
    if (field[iy + 1][ix - 1] < 0 && ix - 1 >= 0 && iy + 1 < 8) {
      out[iy + 1][ix - 1] = 2;
    }
    if (field[iy + 1][ix - 1] == 0 && ix - 1 >= 0 && iy + 1 < 8) {
      out[iy + 1][ix - 1] = 1;
    }
    if (field[iy + 1][ix + 1] < 0 && ix + 1 < 8 && iy + 1 < 8) {
      out[iy + 1][ix + 1] = 2;
    }
    if (field[iy + 1][ix + 1] == 0 && ix + 1 < 8 && iy + 1 < 8) {
      out[iy + 1][ix + 1] = 1;
    }
    if (field[iy + 1][ix] < 0 && iy + 1 < 8) {
      out[iy + 1][ix] = 2;
    }
    if (field[iy + 1][ix] == 0 && iy + 1 < 8) {
      out[iy + 1][ix] = 1;
    }
    if (field[iy - 1][ix] < 0 && iy - 1 >= 0) {
      out[iy - 1][ix] = 2;
    }
    if (field[iy - 1][ix] == 0 && iy - 1 >= 0) {
      out[iy - 1][ix] = 1;
    }
  }
  // black pawn
  if (field[iy][ix] == -1) {
    if (field[iy - 1][ix - 1] > 0 && ix - 1 >= 0 && iy + 1 < 8) {
      out[iy - 1][ix - 1] = 2;
    }
    if (field[iy - 1][ix + 1] > 0 && ix + 1 != 8 && iy + 1 < 8) {
      out[iy - 1][ix + 1] = 2;
    }
    if (field[iy - 1][ix] == 0 && iy + 1 < 8) {
      out[iy - 1][ix] = 1;
    }
    if (field[iy - 2][ix] == 0 && iy == 6 && field[iy - 1][ix] == 0) {
      out[iy - 2][ix] = 1;
    }
  }
  // black horse
  else if (field[iy][ix] == -2) {
    // top
    if (field[iy + 1][ix + 2] > 0 && ix + 2 < 8 && iy + 1 < 8) {
      out[iy + 1][ix + 2] = 2;
    }
    if (field[iy + 1][ix + 2] == 0 && ix + 2 < 8 && iy + 1 < 8) {
      out[iy + 1][ix + 2] = 1;
    }
    if (field[iy + 2][ix + 1] > 0 && ix + 1 < 8 && iy + 2 < 8) {
      out[iy + 2][ix + 1] = 2;
    }
    if (field[iy + 2][ix + 1] == 0 && ix + 1 < 8 && iy + 2 < 8) {
      out[iy + 2][ix + 1] = 1;
    }
    if (field[iy + 1][ix - 2] > 0 && ix - 2 >= 0 && iy + 1 < 8) {
      out[iy + 1][ix - 2] = 2;
    }
    if (field[iy + 1][ix - 2] == 0 && ix - 2 >= 0 && iy + 1 < 8) {
      out[iy + 1][ix - 2] = 1;
    }
    if (field[iy + 2][ix - 1] > 0 && ix - 1 >= 0 && iy + 2 < 8) {
      out[iy + 2][ix - 1] = 2;
    }
    if (field[iy + 2][ix - 1] == 0 && ix - 1 >= 0 && iy + 2 < 8) {
      out[iy + 2][ix - 1] = 1;
    }
    // bot
    if (field[iy - 1][ix + 2] > 0 && ix + 2 < 8 && iy - 1 >= 0) {
      out[iy - 1][ix + 2] = 2;
    }
    if (field[iy - 1][ix + 2] == 0 && ix + 2 < 8 && iy - 1 >= 0) {
      out[iy - 1][ix + 2] = 1;
    }
    if (field[iy - 2][ix + 1] > 0 && ix + 1 < 8 && iy - 2 >= 0) {
      out[iy - 2][ix + 1] = 2;
    }
    if (field[iy - 2][ix + 1] == 0 && ix + 1 < 8 && iy - 2 >= 0) {
      out[iy - 2][ix + 1] = 1;
    }
    if (field[iy - 1][ix - 2] > 0 && ix - 2 >= 0 && iy - 1 >= 0) {
      out[iy - 1][ix - 2] = 2;
    }
    if (field[iy - 1][ix - 2] == 0 && ix - 2 >= 0 && iy - 1 >= 0) {
      out[iy - 1][ix - 2] = 1;
    }
    if (field[iy - 2][ix - 1] > 0 && ix - 1 >= 0 && iy - 2 >= 0) {
      out[iy - 2][ix - 1] = 2;
    }
    if (field[iy - 2][ix - 1] == 0 && ix - 1 >= 0 && iy - 2 >= 0) {
      out[iy - 2][ix - 1] = 1;
    }
  }
  // black slon
  else if (field[iy][ix] == -3) {
    int i;
    int j;
    // top
    for (i = iy + 1, j = ix + 1; i < 8 || j < 8; i++, j++) {
      if (field[i][j] > 0 && i < 8 && j < 8) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i < 8 && j < 8) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    for (i = iy + 1, j = ix - 1; i < 8 || j >= 0; i++, j--) {
      if (field[i][j] > 0 && i < 8 && j >= 0) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i < 8 && j >= 0) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    // bot
    for (i = iy - 1, j = ix + 1; i >= 0 || j < 8; i--, j++) {
      if (field[i][j] > 0 && i >= 0 && j < 8) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i >= 0 && j < 8) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    for (i = iy - 1, j = ix - 1; i >= 0 || j >= 0; i--, j--) {
      if (field[i][j] > 0 && i >= 0 && j >= 0) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i >= 0 && j >= 0) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
  } // black rook
  else if (field[iy][ix] == -4) {
    // top and bot
    for (int i = iy + 1; i < 8; i++) {
      if (field[i][ix] > 0 && i < 8) {
        out[i][ix] = 2;
      }
      if (field[i][ix] == 0 && i < 8) {
        out[i][ix] = 1;
      } else {
        break;
      }
    }
    for (int i = iy - 1; i >= 0; i--) {
      if (field[i][ix] > 0 && i >= 0) {
        out[i][ix] = 2;
      }
      if (field[i][ix] == 0 && i >= 0) {
        out[i][ix] = 1;
      } else {
        break;
      }
    }
    // left and right
    for (int j = ix + 1; j < 8; j++) {
      if (field[iy][j] > 0 && j < 8) {
        out[iy][j] = 2;
      }
      if (field[iy][j] == 0 && j < 8) {
        out[iy][j] = 1;
      } else {
        break;
      }
    }
    for (int j = ix - 1; j >= 0; j--) {
      if (field[iy][j] > 0 && j >= 0) {
        out[iy][j] = 2;
      }
      if (field[iy][j] == 0 && j >= 0) {
        out[iy][j] = 1;
      } else {
        break;
      }
    }
  }
  // black queen
  else if (field[iy][ix] == -5) {
    // top and bot
    for (int i = iy + 1; i < 8; i++) {
      if (field[i][ix] > 0 && i < 8) {
        out[i][ix] = 2;
      }
      if (field[i][ix] == 0 && i < 8) {
        out[i][ix] = 1;
      } else {
        break;
      }
    }
    for (int i = iy - 1; i >= 0; i--) {
      if (field[i][ix] > 0 && i >= 0) {
        out[i][ix] = 2;
      }
      if (field[i][ix] == 0 && i >= 0) {
        out[i][ix] = 1;
      } else {
        break;
      }
    }
    // left and right
    for (int j = ix + 1; j < 8; j++) {
      if (field[iy][j] > 0 && j < 8) {
        out[iy][j] = 2;
      }
      if (field[iy][j] == 0 && j < 8) {
        out[iy][j] = 1;
      } else {
        break;
      }
    }
    for (int j = ix - 1; j >= 0; j--) {
      if (field[iy][j] > 0 && j >= 0) {
        out[iy][j] = 2;
      }
      if (field[iy][j] == 0 && j >= 0) {
        out[iy][j] = 1;
      } else {
        break;
      }
    }
    int i;
    int j;
    // top
    for (i = iy + 1, j = ix + 1; i < 8 || j < 8; i++, j++) {
      if (field[i][j] > 0 && i < 8 && j < 8) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i < 8 && j < 8) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    for (i = iy + 1, j = ix - 1; i < 8 || j >= 0; i++, j--) {
      if (field[i][j] > 0 && i < 8 && j >= 0) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i < 8 && j >= 0) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    // bot
    for (i = iy - 1, j = ix + 1; i >= 0 || j < 8; i--, j++) {
      if (field[i][j] > 0 && i >= 0 && j < 8) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i >= 0 && j < 8) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
    for (i = iy - 1, j = ix - 1; i >= 0 || j >= 0; i--, j--) {
      if (field[i][j] > 0 && i >= 0 && j >= 0) {
        out[i][j] = 2;
        break;
      }
      if (field[i][j] == 0 && i >= 0 && j >= 0) {
        out[i][j] = 1;
      } else {
        break;
      }
    }
  }
  // black king
  else if (field[iy][ix] == -6) {
    if (field[iy][ix - 1] > 0 && ix - 1 >= 0) {
      out[iy][ix - 1] = 2;
    }
    if (field[iy][ix - 1] == 0 && ix - 1 >= 0) {
      out[iy][ix - 1] = 1;
    }
    if (field[iy][ix + 1] > 0 && ix + 1 < 8) {
      out[iy][ix + 1] = 2;
    }
    if (field[iy][ix + 1] == 0 && ix + 1 < 8) {
      out[iy][ix + 1] = 1;
    }
    if (field[iy - 1][ix - 1] > 0 && ix - 1 >= 0 && iy - 1 >= 0) {
      out[iy - 1][ix - 1] = 2;
    }
    if (field[iy - 1][ix - 1] == 0 && ix - 1 >= 0 && iy - 1 >= 0) {
      out[iy - 1][ix - 1] = 1;
    }
    if (field[iy - 1][ix + 1] > 0 && ix + 1 < 8 && iy - 1 >= 0) {
      out[iy - 1][ix + 1] = 2;
    }
    if (field[iy - 1][ix + 1] == 0 && ix + 1 < 8 && iy - 1 >= 0) {
      out[iy - 1][ix + 1] = 1;
    }
    if (field[iy + 1][ix - 1] > 0 && ix - 1 >= 0 && iy + 1 < 8) {
      out[iy + 1][ix - 1] = 2;
    }
    if (field[iy + 1][ix - 1] == 0 && ix - 1 >= 0 && iy + 1 < 8) {
      out[iy + 1][ix - 1] = 1;
    }
    if (field[iy + 1][ix + 1] > 0 && ix + 1 < 8 && iy + 1 < 8) {
      out[iy + 1][ix + 1] = 2;
    }
    if (field[iy + 1][ix + 1] == 0 && ix + 1 < 8 && iy + 1 < 8) {
      out[iy + 1][ix + 1] = 1;
    }
    if (field[iy + 1][ix] > 0 && iy + 1 < 8) {
      out[iy + 1][ix] = 2;
    }
    if (field[iy + 1][ix] == 0 && iy + 1 < 8) {
      out[iy + 1][ix] = 1;
    }
    if (field[iy - 1][ix] > 0 && iy - 1 >= 0) {
      out[iy - 1][ix] = 2;
    }
    if (field[iy - 1][ix] == 0 && iy - 1 >= 0) {
      out[iy - 1][ix] = 1;
    }
  }
}

int
main(int /*argc*/, char** /*argv*/)
{
  engine* engine = create_engine();
  bool isGood = engine->init();
  if (!isGood) {
    destroy_engine(engine);
    return 1;
  }

  std::vector<std::string_view> textures{
    "res/black-brick.png", "res/white-brick.png", "res/black-pawn.png",
    "res/black-horse.png", "res/black-slon.png",  "res/black-rook.png",
    "res/black-queen.png", "res/black-king.png",  "res/white-pawn.png",
    "res/white-horse.png", "res/white-slon.png",  "res/white-rook.png",
    "res/white-queen.png", "res/white-king.png",  "res/green_dot.png"
  };
  if (!engine->load_texture(textures.at(0), 0)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(1), 1)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(2), 2)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(3), 3)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(4), 4)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(5), 5)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(6), 6)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(7), 7)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(8), 8)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(9), 9)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(10), 10)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(11), 11)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(12), 12)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(13), 13)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }
  if (!engine->load_texture(textures.at(14), 14)) {
    std::cout << "failed to load texture" << std::endl;
    return 2;
  }

  std::vector<vertex> grid_bricks;
  float start_y = -1.0f;
  for (int i = 0; i < 9; i++) {
    float start_x = -1.0f;
    for (int j = 0; j < 9; j++) {
      grid_bricks.push_back(
        vertex{ start_x, start_y, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f });
      start_x += 0.25f;
    }
    start_y += 0.25f;
  }

  std::vector<vertex> black_bricks;
  std::vector<vertex> white_bricks;
  cell cells[8][8];
  bool is_black = true;
  for (int i = 0; i < 8; i++) {
    if (i % 2 == 0) {
      is_black = true;
    } else {
      is_black = false;
    }
    for (int j = 0; j < 8; j++) {
      vertex v0 = { grid_bricks.at(i * 9 + j).x,
                    grid_bricks.at(i * 9 + j).y,
                    grid_bricks.at(i * 9 + j).z,
                    grid_bricks.at(i * 9 + j).r,
                    grid_bricks.at(i * 9 + j).g,
                    grid_bricks.at(i * 9 + j).b,
                    grid_bricks.at(i * 9 + j).a,
                    0.0f,
                    1.0f };
      vertex v1 = { grid_bricks.at(i * 9 + j + 1).x,
                    grid_bricks.at(i * 9 + j + 1).y,
                    grid_bricks.at(i * 9 + j + 1).z,
                    grid_bricks.at(i * 9 + j + 1).r,
                    grid_bricks.at(i * 9 + j + 1).g,
                    grid_bricks.at(i * 9 + j + 1).b,
                    grid_bricks.at(i * 9 + j + 1).a,
                    1.0f,
                    1.0f };
      vertex v2 = { grid_bricks.at((1 + i) * 9 + j).x,
                    grid_bricks.at((1 + i) * 9 + j).y,
                    grid_bricks.at((1 + i) * 9 + j).z,
                    grid_bricks.at((1 + i) * 9 + j).r,

                    grid_bricks.at((1 + i) * 9 + j).g,
                    grid_bricks.at((1 + i) * 9 + j).b,
                    grid_bricks.at((1 + i) * 9 + j).a,
                    0.0f,
                    0.0f };
      ;
      vertex v3 = { grid_bricks.at((1 + i) * 9 + j + 1).x,
                    grid_bricks.at((1 + i) * 9 + j + 1).y,
                    grid_bricks.at((1 + i) * 9 + j + 1).z,
                    grid_bricks.at((1 + i) * 9 + j + 1).r,
                    grid_bricks.at((1 + i) * 9 + j + 1).g,
                    grid_bricks.at((1 + i) * 9 + j + 1).b,
                    grid_bricks.at((1 + i) * 9 + j + 1).a,
                    1.0f,
                    0.0f };
      v0.z = 0.5f;
      v1.z = 0.5f;
      v2.z = 0.5f;
      v3.z = 0.5f;
      cells[i][j].v0 = v0;
      cells[i][j].v1 = v1;
      cells[i][j].v2 = v2;
      cells[i][j].v3 = v3;

      if (is_black) {
        add_to_buffer(black_bricks, v0, v1, v2, v3);
        is_black = false;
      } else {
        add_to_buffer(white_bricks, v0, v1, v2, v3);
        is_black = true;
      }
    }
  }

  std::vector<vertex> black_pawn_buffer;
  std::vector<vertex> black_horse_buffer;
  std::vector<vertex> black_rook_buffer;
  std::vector<vertex> black_queen_buffer;
  std::vector<vertex> black_slon_buffer;
  std::vector<vertex> black_king_buffer;
  std::vector<vertex> white_pawn_buffer;
  std::vector<vertex> white_horse_buffer;
  std::vector<vertex> white_rook_buffer;
  std::vector<vertex> white_queen_buffer;
  std::vector<vertex> white_slon_buffer;
  std::vector<vertex> white_king_buffer;
  std::vector<vertex> green_dots_buffer;
  // clang-format off
  int field[8][8] = {
    {4, 2, 3, 5, 6, 3, 2, 4},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {-4, -2, -3, -5, -6, -3, -2, -4}
  };
  // clang-format on
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (field[i][j] == 0) {
        continue;
      } else {
        if (field[i][j] == -1) {
          add_to_buffer(black_pawn_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == -2) {
          add_to_buffer(black_horse_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == -3) {
          add_to_buffer(black_slon_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == -4) {
          add_to_buffer(black_rook_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == -5) {
          add_to_buffer(black_queen_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == -6) {
          add_to_buffer(black_king_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == 1) {
          add_to_buffer(white_pawn_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == 2) {
          add_to_buffer(white_horse_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == 3) {
          add_to_buffer(white_slon_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == 4) {
          add_to_buffer(white_rook_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == 5) {
          add_to_buffer(white_queen_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        } else if (field[i][j] == 6) {
          add_to_buffer(white_king_buffer,
                        cells[i][j].v0,
                        cells[i][j].v1,
                        cells[i][j].v2,
                        cells[i][j].v3);
        }
      }
    }
  }
  int oldix = 0;
  int oldiy = 0;
  bool is_loop = true;
  isGood = false;
  bool is_white = true;
  while (is_loop) {
    event e;
    int way[8][8];
    while (engine->read_input(e)) {
      if (e.type == event::quit) {
        is_loop = false;
      } else if (e.type == event::mouse_click) {

        int ix = e.mouse_x / 100;
        int iy = std::abs(e.mouse_y - 800) / 100;
        if (field[iy][ix] == 0 && isGood == false) {
          continue;
        }
        if (is_white && field[iy][ix] < 0 && !isGood) {
          continue;
        }
        if (!is_white && field[iy][ix] > 0 && !isGood) {
          continue;
        }
        if (isGood == false) {

          find_way(field, ix, iy, way);
          for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
              if (way[i][j] == 1 || way[i][j] == 2) {
                vertex v0 = cells[i][j].v0;
                vertex v1 = cells[i][j].v1;
                vertex v2 = cells[i][j].v2;
                vertex v3 = cells[i][j].v3;
                v0.x += 0.075;
                v0.y += 0.075;
                v1.x -= 0.075;
                v1.y += 0.075;
                v2.x += 0.075;
                v2.y -= 0.075;
                v3.x -= 0.075;
                v3.y -= 0.075;
                add_to_buffer(green_dots_buffer, v0, v1, v2, v3);
              }
            }
          }
          oldix = ix;
          oldiy = iy;
          isGood = true;
        } else if (isGood == true) {
          if (way[iy][ix] == 0) {
            isGood = false;
          } else if (way[iy][ix] == 1 || way[iy][ix] == 2) {
            field[iy][ix] = field[oldiy][oldix];
            field[oldiy][oldix] = 0;

            isGood = false;
            is_white = !is_white;
          }

          green_dots_buffer.clear();
          for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++)
              way[i][j] = 0;
          }
          black_pawn_buffer.clear();
          black_horse_buffer.clear();
          black_slon_buffer.clear();
          black_rook_buffer.clear();
          black_queen_buffer.clear();
          black_king_buffer.clear();
          white_pawn_buffer.clear();
          white_horse_buffer.clear();
          white_slon_buffer.clear();
          white_rook_buffer.clear();
          white_queen_buffer.clear();
          white_king_buffer.clear();
          for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
              if (field[i][j] == 0) {
                continue;
              } else {
                if (field[i][j] == -1) {
                  add_to_buffer(black_pawn_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == -2) {
                  add_to_buffer(black_horse_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == -3) {
                  add_to_buffer(black_slon_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == -4) {
                  add_to_buffer(black_rook_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == -5) {
                  add_to_buffer(black_queen_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == -6) {
                  add_to_buffer(black_king_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == 1) {
                  add_to_buffer(white_pawn_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == 2) {
                  add_to_buffer(white_horse_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == 3) {
                  add_to_buffer(white_slon_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == 4) {
                  add_to_buffer(white_rook_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == 5) {
                  add_to_buffer(white_queen_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                } else if (field[i][j] == 6) {
                  add_to_buffer(white_king_buffer,
                                cells[i][j].v0,
                                cells[i][j].v1,
                                cells[i][j].v2,
                                cells[i][j].v3);
                }
              }
            }
          }
        }
      }
    }
    mat2x3 s;
    engine->render(black_bricks, s, 0);
    engine->render(white_bricks, s, 1);
    engine->render(black_pawn_buffer, s, 2);
    engine->render(black_horse_buffer, s, 3);
    engine->render(black_slon_buffer, s, 4);
    engine->render(black_rook_buffer, s, 5);
    engine->render(black_queen_buffer, s, 6);
    engine->render(black_king_buffer, s, 7);
    engine->render(white_pawn_buffer, s, 8);
    engine->render(white_horse_buffer, s, 9);
    engine->render(white_slon_buffer, s, 10);
    engine->render(white_rook_buffer, s, 11);
    engine->render(white_queen_buffer, s, 12);
    engine->render(white_king_buffer, s, 13);
    engine->render(green_dots_buffer, s, 14);
    engine->swap_buffers();
  }
  engine->destroy();
  destroy_engine(engine);
  return 0;
}
