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
#define HEURISTIC_STRATEGY HEURISTIC_STRATEGY_TAXI_DIST
#define PF_CELL_IDX(x, y) (y * cells_w + x)

#define PF_CELL_FLAG_VISITED 0b1

struct PathFinder {
  u_int8_t cells[MAX_GRID_CELLS]{};
  int cells_w{};
  int cells_h{};

  PathFinder() {
  }
  ~PathFinder() {
  }

  void find_path(Vector2 start, Vector2 end) const {
    static int8_t neighbor_map[8][2]{
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1},
    };

    IntVector2 start_normalized = IntVector2{mod_reduce(start.x, CELL_DISTANCE), mod_reduce(start.y, CELL_DISTANCE)};
    IntVector2 end_normalized = IntVector2{mod_reduce(end.x, CELL_DISTANCE), mod_reduce(end.y, CELL_DISTANCE)};

    std::vector<PFCell> queue{};
    queue.emplace_back(0.f, heuristic_distance(start_normalized, end_normalized), start_normalized);

    u_int8_t visited_cells[MAX_GRID_CELLS]{};
    visited_cells[PF_CELL_IDX(start_normalized.x, start_normalized.y)] |= PF_CELL_FLAG_VISITED;

    while (!queue.empty()) {
      // Get the most promising cell.
      std::pop_heap(queue.begin(), queue.end(), std::greater{});
      PFCell min_cell = queue.front();
      queue.pop_back();

      if (min_cell.p == end_normalized) {
        TraceLog(LOG_INFO, "Found path");
      }

      for (auto &neighbor_offs : neighbor_map) {
        IntVector2 neighbor_coord{min_cell.p.x + neighbor_offs[0], min_cell.p.y + neighbor_offs[1]};

        // Out of bounds.
        if (neighbor_coord.x < 0 || neighbor_coord.y < 0 || neighbor_coord.x > cells_w || neighbor_coord.y > cells_h)
          continue;
        // Already visited.
        if ((visited_cells[PF_CELL_IDX(neighbor_coord.x, neighbor_coord.y)] & PF_CELL_FLAG_VISITED) > 0) continue;

        // Calculate and save new cell.
        queue.emplace_back(heuristic_distance(min_cell.p, neighbor_coord),
                           heuristic_distance(neighbor_coord, end_normalized), neighbor_coord);
        visited_cells[PF_CELL_IDX(neighbor_coord.x, neighbor_coord.y)] |= PF_CELL_FLAG_VISITED;
      }
    }

    TraceLog(LOG_WARNING, "Did not find a path!");
  }

  float heuristic_distance(IntVector2 lhs, IntVector2 rhs) const {
    switch (HEURISTIC_STRATEGY) {
      case HEURISTIC_STRATEGY_TAXI_DIST:
        return (float)abs(lhs.x - rhs.x) + abs(lhs.y - rhs.y);
      default:
        TraceLog(LOG_ERROR, "Invalid heuristic strategy.");
        exit(EXIT_FAILURE);
    }
  }
};
