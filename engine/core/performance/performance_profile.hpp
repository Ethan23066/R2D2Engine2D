#pragma once

#include <algorithm>
#include <cstdint>
#include <string>

namespace r2d2 {

struct PerformanceProfile {
    int recommended_fps = 60;
    int monitor_refresh_hz = 60;
    int cpu_cores = 0;
    int ram_gb = 0;
    float gpu_score = 0.0f;
    float render_scale = 1.0f;
    bool vsync = true;
    std::string quality = "medium";
};

inline PerformanceProfile detect_performance_profile(int monitor_refresh_hz = 60,
                                                    int cpu_cores = 0,
                                                    int ram_gb = 0,
                                                    float gpu_score = 0.0f,
                                                    int window_width = 1280,
                                                    int window_height = 720) {
    PerformanceProfile profile;
    profile.monitor_refresh_hz = std::max(30, monitor_refresh_hz);
    profile.cpu_cores = cpu_cores > 0 ? cpu_cores : 1;
    profile.ram_gb = ram_gb > 0 ? ram_gb : 8;
    profile.gpu_score = std::max(0.0f, gpu_score);

    const int pixel_count = window_width * window_height;
    if (pixel_count > 2'000'000) {
        profile.render_scale = 0.85f;
    } else if (pixel_count > 1'000'000) {
        profile.render_scale = 0.95f;
    } else {
        profile.render_scale = 1.0f;
    }

    if (profile.gpu_score < 0.35f || profile.cpu_cores <= 2 || profile.ram_gb <= 8) {
        profile.recommended_fps = 30;
        profile.quality = "low";
        profile.vsync = true;
    } else if (profile.gpu_score < 0.7f || profile.cpu_cores <= 4 || profile.ram_gb <= 16) {
        profile.recommended_fps = 60;
        profile.quality = "medium";
        profile.vsync = true;
    } else if (profile.gpu_score < 1.0f) {
        profile.recommended_fps = std::min(90, profile.monitor_refresh_hz);
        profile.quality = "high";
        profile.vsync = true;
    } else {
        profile.recommended_fps = std::min(120, profile.monitor_refresh_hz);
        profile.quality = "high";
        profile.vsync = false;
    }

    profile.recommended_fps = std::max(30, std::min(profile.recommended_fps, profile.monitor_refresh_hz));
    return profile;
}

inline std::string describe_profile(const PerformanceProfile& profile) {
    return "recommended_fps=" + std::to_string(profile.recommended_fps) +
           ", quality=" + profile.quality +
           ", render_scale=" + std::to_string(profile.render_scale) +
           ", vsync=" + std::string(profile.vsync ? "on" : "off");
}

} // namespace r2d2
