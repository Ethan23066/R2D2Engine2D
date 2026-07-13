#pragma once

#include <atomic>

namespace r2d2 {

class Lifecycle {
public:
    Lifecycle();

    // Interdire copie
    Lifecycle(const Lifecycle&) = delete;
    Lifecycle& operator=(const Lifecycle&) = delete;

    // Interdire move
    Lifecycle(Lifecycle&&) = delete;
    Lifecycle& operator=(Lifecycle&&) = delete;

    void init();
    void update();
    void shutdown();
    uint8_t is_running() const;

private:
    std::atomic<bool> running_;
};

} // namespace r2d2
