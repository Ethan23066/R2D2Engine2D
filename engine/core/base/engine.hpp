// engine/core/base/engine.hpp

#pragma once

namespace r2d2 {

    class Engine {
    public:
        Engine();
        ~Engine();

        void init();
        void shutdown();
        void tick();

        bool is_running() const;

    private:
        bool running;
    };

}
