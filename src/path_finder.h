#pragma once

#include <stdlib.h>

#include "map.h"
#include "raylib.h"

#define MAX_GRID_CELLS 10000
#define CELL_DISTANCE 50

void record_available_cells(Map const& map, u_int8_t* cells, int* cells_w, int* cells_h) {
  *cells_w = map.map_image.width / CELL_DISTANCE;
  *cells_h = map.map_image.height / CELL_DISTANCE;

  if (*cells_w * *cells_w >= MAX_GRID_CELLS) {
    TraceLog(LOG_ERROR, "Map too large");
    exit(EXIT_FAILURE);
  }

  for (int y = 0; y < *cells_h; y++) {
    for (int x = 0; x < *cells_w; x++) {
      if (map.is_hit(Vector2{(float)(x * CELL_DISTANCE), (float)(y * CELL_DISTANCE)})) {
        cells[y * CELL_DISTANCE + x] = 0b1;
      } else {
        cells[y * CELL_DISTANCE + x] = 0b0;
      }
    }
  }
}

struct PathFinder {
  u_int8_t cells[MAX_GRID_CELLS]{};
  int cells_w{};
  int cells_h{};

  PathFinder() {
  }
  ~PathFinder() {
  }

  void init(Map const& map) {
    record_available_cells(map, cells, &cells_w, &cells_h);
  }
};
