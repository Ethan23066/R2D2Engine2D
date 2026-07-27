#include "engine_window.hpp"
#include <iostream>

bool engine_window_init(EngineWindowConfig cfg)
{
    std::cout << "[engine_window] init stub: "
              << cfg.width << "x" << cfg.height
              << " title=" << cfg.title << "\n";
    return true;
}

void engine_window_run()
{
    std::cout << "[engine_window] run stub\n";
}

void engine_window_shutdown()
{
    std::cout << "[engine_window] shutdown stub\n";
}
