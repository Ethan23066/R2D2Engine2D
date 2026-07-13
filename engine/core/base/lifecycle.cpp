#include "lifecycle.hpp"

namespace r2d2 {

Lifecycle::Lifecycle()
    : running_(false)
{}

void Lifecycle::init() {
    running_.store(true);
}

void Lifecycle::update() {
    // plus tard : update du monde, ECS, physics, etc.
}

void Lifecycle::shutdown() {
    running_.store(false);
}

uint8_t Lifecycle::is_running() const {
    return running_ ? 1 : 0;
}


} // namespace r2d2
