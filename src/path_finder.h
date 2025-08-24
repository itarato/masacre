#pragma once

#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "common.h"
#include "raylib.h"

#define MAX_GRID_CELLS 10000
#define CELL_DISTANCE 50

#define HEURISTIC_STRATEGY_TAXI_DIST 0b00

struct PathFinder {
  u_int8_t cells[MAX_GRID_CELLS]{};
  int cells_w{};
  int cells_h{};

  PathFinder() {
  }
  ~PathFinder() {
  }

  void find_path(Vector2 start, Vector2 end) const {
    IntVector2 start_normalized = IntVector2{mod_reduce(start.x, CELL_DISTANCE), mod_reduce(start.y, CELL_DISTANCE)};
    IntVector2 end_normalized = IntVector2{mod_reduce(end.x, CELL_DISTANCE), mod_reduce(end.y, CELL_DISTANCE)};

    std::vector<PFCell> queue{};
    queue.push_back(PFCell{0, heuristic_distance(start_normalized, end_normalized, HEURISTIC_STRATEGY_TAXI_DIST),
                           start_normalized});

    while (!queue.empty()) {
      std::pop_heap(queue.begin(), queue.end(), std::greater{});
      PFCell min_cell = queue.front();
      queue.pop_back();
    }

    TraceLog(LOG_ERROR, "Did not find a path!");
  }

  int heuristic_distance(IntVector2 lhs, IntVector2 rhs, int heuristic_strategy) const {
    switch (heuristic_strategy) {
      case HEURISTIC_STRATEGY_TAXI_DIST:
        return abs(lhs.x - rhs.x) + abs(lhs.y - rhs.y);
      default:
        TraceLog(LOG_ERROR, "Invalid heuristic strategy.");
        exit(EXIT_FAILURE);
    }
  }
};
