#include "lifecycle.hpp"

namespace r2d2 {

Lifecycle::Lifecycle() = default;

void Lifecycle::init() {
    running_.store(true, std::memory_order_release);
}

void Lifecycle::update() {
    // plus tard : update du monde, ECS, physics, etc.
}

void Lifecycle::shutdown() {
    running_.store(false, std::memory_order_release);
}

uint8_t Lifecycle::is_running() const {
    return running_.load(std::memory_order_acquire) ? 1U : 0U;
}

} // namespace r2d2
