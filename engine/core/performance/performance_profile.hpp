#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <vector>

namespace r2d2 {

struct BenchmarkResult {
    double avg_frame_ms = 0.0;
    double p95_frame_ms = 0.0;
    int estimated_max_fps = 60;
    int recommended_fps = 60;
    float render_scale = 1.0f;
};

template <typename WorkloadFn>
[[nodiscard]] inline BenchmarkResult run_frame_benchmark(
    WorkloadFn&& workload,
    int monitor_refresh_hz = 60,
    int warmup_frames = 5,
    int sample_frames = 30) {

    if (warmup_frames < 0)
        throw std::invalid_argument("warmup_frames cannot be negative");
    if (sample_frames <= 0)
        throw std::invalid_argument("sample_frames must be greater than zero");

    using clock = std::chrono::steady_clock;

    for (int i = 0; i < warmup_frames; ++i)
        workload();

    std::vector<double> samples_ms;
    samples_ms.reserve(static_cast<std::size_t>(sample_frames));

    for (int i = 0; i < sample_frames; ++i) {
        const auto start = clock::now();
        workload();
        const auto end = clock::now();

        const double ms = std::chrono::duration<double, std::milli>(end - start).count();
        samples_ms.push_back(ms);
    }

    std::sort(samples_ms.begin(), samples_ms.end());

    const double sum = std::accumulate(samples_ms.begin(), samples_ms.end(), 0.0);
    const double avg_ms = sum / static_cast<double>(samples_ms.size());
    const auto p95_rank = static_cast<std::size_t>(
        std::ceil(static_cast<double>(samples_ms.size()) * 0.95));
    const std::size_t p95_idx = std::max<std::size_t>(1, p95_rank) - 1;
    const double p95_ms = samples_ms[p95_idx];

    const int safe_hz = std::max(30, monitor_refresh_hz);
    const double estimated_fps = p95_ms > 0.001 ? 1000.0 / p95_ms : 1000.0;

    BenchmarkResult res;
    res.avg_frame_ms = avg_ms;
    res.p95_frame_ms = p95_ms;
    res.estimated_max_fps = estimated_fps >= static_cast<double>(std::numeric_limits<int>::max())
        ? std::numeric_limits<int>::max()
        : static_cast<int>(estimated_fps);

    // Keep a 20% timing margin against the display refresh budget.
    if (estimated_fps >= static_cast<double>(safe_hz) * 1.2) {
        res.recommended_fps = safe_hz;
        res.render_scale = 1.0f;
    } else if (estimated_fps >= 60.0) {
        res.recommended_fps = 60;
        res.render_scale = 0.95f;
    } else {
        res.recommended_fps = 30;
        res.render_scale = 0.85f;
    }

    return res;
}

} // namespace r2d2
