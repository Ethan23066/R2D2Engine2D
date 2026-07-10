#include "config.hpp"

namespace r2d2 {

EngineConfig::EngineConfig()
    : window_width(1280),
      window_height(720),
      vsync(true),
      fullscreen(false),
      title("R2D2Engine2D")
{}

void load_default_config(EngineConfig& cfg) {
    cfg = EngineConfig{};
}

} // namespace r2d2
