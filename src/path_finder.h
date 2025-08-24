#pragma once

#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "common.h"
#include "raylib.h"

#define MAX_GRID_CELLS 10000

#define HEURISTIC_STRATEGY_TAXI_DIST 0b00
#define HEURISTIC_STRATEGY HEURISTIC_STRATEGY_TAXI_DIST
#define PF_CELL_IDX(x, y) (y * cells_w + x)

#define PF_CELL_FLAG_VISITED 0b1

#define PF_CELL_ACCESSIBLE_FLAG 0b1

struct PathFinder {
  u_int8_t cells[MAX_GRID_CELLS]{};
  int cells_w{};
  int cells_h{};

  PathFinder() {
  }
  ~PathFinder() {
  }

  void find_path(Vector2 start, Vector2 end) const {
    IntVector2 start_normalized = cell_idx_from_coord(start);
    IntVector2 end_normalized = cell_idx_from_coord(end);
    return find_path(start_normalized, end_normalized);
  }

  void find_path(IntVector2 start, IntVector2 end) const {
    static int8_t neighbor_map[8][2]{
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1},
    };

    if (is_out_of_bounds(start)) {
      TraceLog(LOG_ERROR, "Out of bound start for path finder: %d:%d.", start.x, start.y);
      return;
    }

    if (is_out_of_bounds(end)) {
      TraceLog(LOG_ERROR, "Out of bound end for path finder: %d:%d.", end.x, end.y);
      return;
    }

    std::vector<PFCell> queue{};
    queue.emplace_back(0.f, heuristic_distance(start, end), start);

    u_int8_t visited_cells[MAX_GRID_CELLS]{};
    visited_cells[PF_CELL_IDX(start.x, start.y)] |= PF_CELL_FLAG_VISITED;

    while (!queue.empty()) {
      // Get the most promising cell.
      std::pop_heap(queue.begin(), queue.end(), std::greater{});
      PFCell min_cell = queue.back();
      queue.pop_back();

      TraceLog(LOG_DEBUG, "Min cell: %d:%d G=%.2f H=%.2f F=%.2f", min_cell.p.x, min_cell.p.y, min_cell.prefix,
               min_cell.suffix, min_cell.total());

      if (min_cell.p == end) {
        TraceLog(LOG_DEBUG, "Found path");
        return;
      }

      for (auto &neighbor_offs : neighbor_map) {
        IntVector2 neighbor_coord{min_cell.p.x + neighbor_offs[0], min_cell.p.y + neighbor_offs[1]};

        // Out of bounds.
        if (is_out_of_bounds(neighbor_coord)) continue;
        // Not accessible.
        if (!is_accessible(neighbor_coord)) continue;
        // Already visited.
        if ((visited_cells[PF_CELL_IDX(neighbor_coord.x, neighbor_coord.y)] & PF_CELL_FLAG_VISITED) > 0) continue;

        // Calculate and save new cell.
        queue.emplace_back(heuristic_distance(min_cell.p, neighbor_coord), heuristic_distance(neighbor_coord, end),
                           neighbor_coord);
        std::push_heap(queue.begin(), queue.end(), std::greater{});

        TraceLog(LOG_DEBUG, "- pushed: %d:%d", neighbor_coord.x, neighbor_coord.y);

        visited_cells[PF_CELL_IDX(neighbor_coord.x, neighbor_coord.y)] |= PF_CELL_FLAG_VISITED;
      }
    }

    TraceLog(LOG_WARNING, "Did not find a path for %d:%d -> %d:%d", start.x, start.y, end.x, end.y);
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

  bool is_out_of_bounds(IntVector2 const &p) const {
    return p.x < 0 || p.y < 0 || p.x > cells_w || p.y > cells_h;
  }

  bool is_accessible(IntVector2 const &p) const {
    return (cells[PF_CELL_IDX(p.x, p.y)] & PF_CELL_ACCESSIBLE_FLAG) > 0;
  }
};
