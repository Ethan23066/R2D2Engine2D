#pragma once

class RendererGL3;

class RendererManager {
public:
    RendererManager();
    ~RendererManager();

    bool init();
    void resize(int w, int h);
    void begin_frame();
    void end_frame();
    void shutdown();

private:
    RendererGL3* backend;
};
