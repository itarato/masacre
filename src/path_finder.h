#pragma once

#include <stdlib.h>

#include "raylib.h"

#define MAX_GRID_CELLS 10000
#define CELL_DISTANCE 50

struct PathFinder {
  u_int8_t cells[MAX_GRID_CELLS]{};
  int cells_w{};
  int cells_h{};

  PathFinder() {
  }
  ~PathFinder() {
  }
};
