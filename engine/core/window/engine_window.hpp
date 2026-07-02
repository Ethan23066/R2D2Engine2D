#ifndef ENGINE_WINDOW_HPP
#define ENGINE_WINDOW_HPP

struct EngineWindowConfig {
    int width;
    int height;
    const char* title;
};

bool engine_window_init(EngineWindowConfig cfg);
void engine_window_run();
void engine_window_shutdown();

#endif
