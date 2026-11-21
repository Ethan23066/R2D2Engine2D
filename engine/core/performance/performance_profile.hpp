#pragma once

#include <algorithm>
#include <chrono>
#include <numeric>
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

    using clock = std::chrono::steady_clock;

    for (int i = 0; i < warmup_frames; ++i) {
        workload();
    }

    std::vector<double> samples_ms;
    samples_ms.reserve(sample_frames);

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
    const std::size_t p95_idx = static_cast<std::size_t>(samples_ms.size() * 0.95);
    const double p95_ms = samples_ms[std::min(p95_idx, samples_ms.size() - 1)];

    const int safe_hz = std::max(30, monitor_refresh_hz);

    BenchmarkResult res;
    res.avg_frame_ms = avg_ms;
    res.p95_frame_ms = p95_ms;
    res.estimated_max_fps = p95_ms > 0.001
        ? static_cast<int>(1000.0 / p95_ms)
        : 1000;

    // On garde une marge de sécurité de 20 % sur le budget temps
    if (res.estimated_max_fps >= static_cast<int>(safe_hz * 1.2)) {
        res.recommended_fps = safe_hz;
        res.render_scale = 1.0f;
    } else if (res.estimated_max_fps >= 60) {
        res.recommended_fps = 60;
        res.render_scale = 0.95f;
    } else {
        res.recommended_fps = 30;
        res.render_scale = 0.85f;
    }

    return res;
}

} // namespace r2d2
