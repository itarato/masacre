#pragma once

#include <raylib.h>

#include <algorithm>
#include <deque>

#define PERF_CHART_MAX_VALUES 100
#define PERF_CHART_BAR_HEIGHT 80.f
#define PERF_CHART_BAR_WIDTH 4.f
#define PERF_SAMPLE_RATE 4

struct PerfChart {
  std::deque<float> values{};
  float active_frame_start{};
  float accumulated_time{0.f};
  int sample_count{0};

  void register_active_frame_start() {
    active_frame_start = GetTime();
  }

  void register_active_frame_end() {
    accumulated_time += GetTime() - active_frame_start;
    sample_count++;

    if (sample_count >= PERF_SAMPLE_RATE) {
      values.push_back(accumulated_time / PERF_SAMPLE_RATE);
      accumulated_time = 0.f;
      sample_count = 0;
    }
  }

  void update() {
    ensure_capacity();
  }

  void draw() const {
    if (values.empty()) return;

    float _min = min();
    float _max = max();
    float _min_adjusted = min() * 0.9f;
    float _max_adjusted = max() * 1.1f;

    int offset = PERF_CHART_MAX_VALUES - values.size();

    DrawRectangle(4, GetScreenHeight() - 12 - PERF_CHART_BAR_HEIGHT,
                  PERF_CHART_MAX_VALUES * PERF_CHART_BAR_WIDTH + 8 + 80, PERF_CHART_BAR_HEIGHT + 8,
                  ColorAlpha(BLACK, 0.4));
    for (unsigned int i = 0; i < values.size(); i++) {
      float height = ((values[i] - _min_adjusted) / (_max_adjusted - _min_adjusted)) * PERF_CHART_BAR_HEIGHT;
      DrawRectangle(8.f + ((offset + i) * PERF_CHART_BAR_WIDTH), GetScreenHeight() - 8.f - height, PERF_CHART_BAR_WIDTH,
                    height, MAROON);
    }

    DrawText(TextFormat("%.2f ms", _max * 1000), 12 + PERF_CHART_MAX_VALUES * PERF_CHART_BAR_WIDTH,
             GetScreenHeight() - 8 - PERF_CHART_BAR_HEIGHT, 20, WHITE);
    DrawText(TextFormat("%.2f ms", _min * 1000), 12 + PERF_CHART_MAX_VALUES * PERF_CHART_BAR_WIDTH,
             GetScreenHeight() - 28, 20, WHITE);
  }

 private:
  [[nodiscard]] float min() const {
    return std::ranges::min(values);
  }

  [[nodiscard]] float max() const {
    return std::ranges::max(values);
  }

  void ensure_capacity() {
    if (values.size() > PERF_CHART_MAX_VALUES) values.pop_front();
  }
};
