#pragma once

#include <string>

namespace r2d2 {

struct EngineConfig {
    int window_width;
    int window_height;
    bool vsync;
    bool fullscreen;
    std::string title;

    EngineConfig();
};

void load_default_config(EngineConfig& cfg);

} // namespace r2d2
