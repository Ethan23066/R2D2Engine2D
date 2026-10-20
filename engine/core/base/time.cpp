#include "time.hpp"

namespace r2d2 {

Time::Time() {
    last = std::chrono::steady_clock::now();
    current = last;
    delta_time = 0.0;
}

void Time::update() {
    const auto now = std::chrono::steady_clock::now();
    const std::chrono::duration<double> duration = now - last;
    delta_time = duration.count();
    last = now;
    current = now;
}

double Time::delta() const {
    return delta_time;
}

double Time::now() const {
    return std::chrono::duration<double>(current.time_since_epoch()).count();
}

} // namespace r2d2
