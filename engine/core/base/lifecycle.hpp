#pragma once

#include <atomic>

namespace r2d2 {

class Lifecycle {
public:
    Lifecycle();

    Lifecycle(const Lifecycle&) = delete;
    Lifecycle& operator=(const Lifecycle&) = delete;
    Lifecycle(Lifecycle&&) = delete;
    Lifecycle& operator=(Lifecycle&&) = delete;

    void init();
    void update();
    void shutdown();
    uint8_t is_running() const;

private:
    std::atomic<bool> running_{false};
};

} // namespace r2d2
