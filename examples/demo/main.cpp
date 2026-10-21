#include "engine/core/backend/gl3/RendererGL3.hpp"
#include "engine/core/window/window.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

int main() {
    EngineWindowConfig cfg{1280, 720, "R2D2Engine2D Demo", false, true};
    if (!engine_window_init(&cfg, 0)) {
        std::cerr << "failed to initialize window\n";
        return 1;
    }

    void* native = engine_window_get_native_handle();
    if (!native) {
        std::cerr << "failed to get native window handle\n";
        engine_window_shutdown();
        return 1;
    }

    RendererGL3 renderer;
    if (!renderer.init(native, cfg.width, cfg.height)) {
        std::cerr << "failed to initialize OpenGL renderer\n";
        engine_window_shutdown();
        return 1;
    }

    while (!engine_window_should_close()) {
        engine_window_poll_events();
        renderer.begin_frame(0.1f, 0.12f, 0.15f, 1.0f);
        renderer.end_frame();
    }

    renderer.shutdown();
    engine_window_shutdown();
    return 0;
}
