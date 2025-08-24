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

static int8_t NEIGHBOR_MAP[8][2]{
    {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1},
};

struct PathFinder {
  u_int8_t cells[MAX_GRID_CELLS]{};
  int cells_w{};
  int cells_h{};

  PathFinder() {
  }
  ~PathFinder() {
  }

  std::vector<IntVector2> find_path(Vector2 start, Vector2 end) const {
    IntVector2 start_normalized = cell_idx_from_coord(start);
    IntVector2 end_normalized = cell_idx_from_coord(end);
    return find_path(start_normalized, end_normalized);
  }

  std::vector<IntVector2> find_path(IntVector2 start, IntVector2 end) const {
    if (is_out_of_bounds(start)) {
      TraceLog(LOG_ERROR, "Out of bound start for path finder: %d:%d.", start.x, start.y);
      return {};
    }

    if (is_out_of_bounds(end)) {
      TraceLog(LOG_ERROR, "Out of bound end for path finder: %d:%d.", end.x, end.y);
      return {};
    }

    std::vector<PFCell> queue{};
    queue.emplace_back(0.f, heuristic_distance(start, end), start);

    // Visited byte setup:
    //        ┌ source (index of neighbour_map)
    //        |  ┌ visited (1: yes, 0: no)
    // 0bxxxx_bbba
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
        return backtrack_path(visited_cells, start, end);
      }

      for (u_int8_t i = 0; i < 8; i++) {
        auto neighbor_offs = NEIGHBOR_MAP[i];
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

        u_int8_t visited_bits = PF_CELL_FLAG_VISITED | ((i & 0b111) << 1);
        visited_cells[PF_CELL_IDX(neighbor_coord.x, neighbor_coord.y)] = visited_bits;
      }
    }

    TraceLog(LOG_WARNING, "Did not find a path for %d:%d -> %d:%d", start.x, start.y, end.x, end.y);
    return {};
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

  std::vector<IntVector2> backtrack_path(u_int8_t *visited_cells, IntVector2 const &start,
                                         IntVector2 const &end) const {
    std::vector<IntVector2> out{};
    IntVector2 current_coord = end;

    out.push_back(end);

    while (current_coord != start) {
      u_int8_t origin_index = (visited_cells[PF_CELL_IDX(current_coord.x, current_coord.y)] >> 1) & 0b111;
      auto origin_offset = NEIGHBOR_MAP[origin_index];
      current_coord.x -= origin_offset[0];
      current_coord.y -= origin_offset[1];

      out.push_back(current_coord);
    }

    return out;
  }
};
