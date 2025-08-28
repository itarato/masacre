#pragma once

#include <algorithm>
#include <cmath>
#include <queue>
#include <ranges>
#include <vector>

#include "common.h"
#include "raylib.h"

#define MAX_GRID_CELLS 10000

#define HEURISTIC_STRATEGY_TAXI_DIST 0b00
#define HEURISTIC_STRATEGY_EUCLIDEAN_DIST 0b01
#define HEURISTIC_STRATEGY HEURISTIC_STRATEGY_EUCLIDEAN_DIST

#define PF_CELL_IDX(x, y) (y * cells_w + x)

#define PF_CELL_FLAG_VISITED 0b1

// Cells that are not on a hit zone.
#define PF_CELL_ACCESSIBLE_FLAG 0b01
// Cells that are discoverable from `start_pos`.
#define PF_CELL_DISCOVERABLE_FLAG 0b10

#define PF_RANDOM_SPOT_MAX_ATTEMPTS 32

static int8_t NEIGHBOR_MAP[8][2]{
    {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1},
};

struct PathFinder {
  u_int8_t cells[MAX_GRID_CELLS]{};
  int cells_w{};
  int cells_h{};

  IntVector2 start_pos{};

  PathFinder() {
  }
  ~PathFinder() {
  }

  void post_init() {
    init_start_pos();
    init_discoverable_cells();
  }

  std::vector<IntVector2> find_path(Vector2 start, Vector2 end) const {
    IntVector2 start_normalized = closest_available_cell_idx_from_coord(start);
    IntVector2 end_normalized = closest_available_cell_idx_from_coord(end);
    return find_path(start_normalized, end_normalized);
  }

  std::vector<IntVector2> find_path(IntVector2 start, IntVector2 end) const {
    if (is_out_of_bounds(start)) {
      TraceLog(LOG_ERROR, "[PF] start is out of bound: %d:%d.", start.x, start.y);
      return {};
    }

    if (!is_accessible(start)) {
      TraceLog(LOG_ERROR, "[PF] start is not accessible: %d:%d.", start.x, start.y);
      return {};
    }

    if (!is_discoverable(start)) {
      TraceLog(LOG_ERROR, "[PF] start is not discoverable: %d:%d.", start.x, start.y);
      return {};
    }

    if (is_out_of_bounds(end)) {
      TraceLog(LOG_ERROR, "[PF] end is out of bound: %d:%d.", end.x, end.y);
      return {};
    }

    if (!is_accessible(end)) {
      TraceLog(LOG_ERROR, "[PF] end is not accessible: %d:%d.", end.x, end.y);
      return {};
    }

    if (!is_discoverable(end)) {
      TraceLog(LOG_ERROR, "[PF] end is not discoverable: %d:%d.", end.x, end.y);
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
      std::ranges::pop_heap(queue, std::greater{});
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
        std::ranges::push_heap(queue, std::greater{});

        TraceLog(LOG_DEBUG, "- pushed: %d:%d", neighbor_coord.x, neighbor_coord.y);

        u_int8_t visited_bits = PF_CELL_FLAG_VISITED | ((i & 0b111) << 1);
        visited_cells[PF_CELL_IDX(neighbor_coord.x, neighbor_coord.y)] = visited_bits;
      }
    }

    TraceLog(LOG_WARNING, "Did not find a path for %d:%d -> %d:%d", start.x, start.y, end.x, end.y);
    return {};
  }

  IntVector2 closest_available_cell_idx_from_coord(Vector2 coord) const {
    auto possible_cells = ordered_cell_indices_from_coord(coord);
    for (auto const &cell : possible_cells) {
      if (!is_out_of_bounds(cell) && is_discoverable(cell)) return cell;
    }

    TraceLog(LOG_ERROR, "No closest accessible cell found for %.2f:%.2f: %d:%d %d:%d %d:%d %d:%d", coord.x, coord.y,
             possible_cells[0].x * CELL_DISTANCE, possible_cells[0].y * CELL_DISTANCE,
             possible_cells[1].x * CELL_DISTANCE, possible_cells[1].y * CELL_DISTANCE,
             possible_cells[2].x * CELL_DISTANCE, possible_cells[2].y * CELL_DISTANCE,
             possible_cells[3].x * CELL_DISTANCE, possible_cells[3].y * CELL_DISTANCE);
    return possible_cells.front();
  }

  float heuristic_distance(IntVector2 lhs, IntVector2 rhs) const {
    switch (HEURISTIC_STRATEGY) {
      case HEURISTIC_STRATEGY_TAXI_DIST:
        return (float)abs(lhs.x - rhs.x) + abs(lhs.y - rhs.y);
      case HEURISTIC_STRATEGY_EUCLIDEAN_DIST:
        return int_vector2_dist(lhs, rhs);
      default:
        TraceLog(LOG_ERROR, "Invalid heuristic strategy.");
        exit(EXIT_FAILURE);
    }
  }

  IntVector2 discoverable_random_spot() const {
    IntVector2 pos;

    for (int i = 0; i < PF_RANDOM_SPOT_MAX_ATTEMPTS; i++) {
      pos = IntVector2{rand() % cells_w, rand() % cells_h};

      if (is_discoverable(pos)) return pos;
    }

    TraceLog(LOG_ERROR, "No available random spot");
    exit(EXIT_FAILURE);
  }

  void draw(Vector2 world_offset) const {
    // draw_cells_debug(world_offset);
  }

 private:
  bool is_out_of_bounds(IntVector2 const &p) const {
    return p.x < 0 || p.y < 0 || p.x >= cells_w || p.y >= cells_h;
  }

  bool is_accessible(IntVector2 const &p) const {
    return (cells[PF_CELL_IDX(p.x, p.y)] & PF_CELL_ACCESSIBLE_FLAG) > 0;
  }

  bool is_discoverable(IntVector2 const &p) const {
    return (cells[PF_CELL_IDX(p.x, p.y)] & PF_CELL_DISCOVERABLE_FLAG) > 0;
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

  void init_start_pos() {
    int x = cells_w / 2;
    for (int y = cells_h / 2; y >= 0; y++) {
      if ((cells[PF_CELL_IDX(x, y)] & PF_CELL_ACCESSIBLE_FLAG) > 0) {
        start_pos = {x, y};
        return;
      }
    }

    TraceLog(LOG_ERROR, "No start position found.");
    exit(EXIT_FAILURE);
  }

  void init_discoverable_cells() {
    std::queue<IntVector2> queue{};
    queue.push(start_pos);

    cells[PF_CELL_IDX(start_pos.x, start_pos.y)] |= PF_CELL_DISCOVERABLE_FLAG;

    while (!queue.empty()) {
      IntVector2 current_pos = queue.front();
      queue.pop();

      for (auto const &neighbor_offs : NEIGHBOR_MAP) {
        IntVector2 neighbor_pos{current_pos.x + neighbor_offs[0], current_pos.y + neighbor_offs[1]};
        if (is_out_of_bounds(neighbor_pos)) continue;
        if (!is_accessible(neighbor_pos)) continue;
        if (is_discoverable(neighbor_pos)) continue;

        cells[PF_CELL_IDX(neighbor_pos.x, neighbor_pos.y)] |= PF_CELL_DISCOVERABLE_FLAG;
        queue.push(neighbor_pos);
      }
    }
  }

  void draw_cells_debug(Vector2 const &world_offset) const {
    for (int y = 0; y < cells_h; y++) {
      for (int x = 0; x < cells_w; x++) {
        auto color = BLACK;
        if ((cells[PF_CELL_IDX(x, y)] & PF_CELL_DISCOVERABLE_FLAG) > 0) {
          color = GREEN;
        } else if ((cells[PF_CELL_IDX(x, y)] & PF_CELL_ACCESSIBLE_FLAG) > 0) {
          color = BLUE;
        }
        DrawCircle(x * CELL_DISTANCE + world_offset.x, y * CELL_DISTANCE + world_offset.y, 4.f, color);
      }
    }
  }
};
