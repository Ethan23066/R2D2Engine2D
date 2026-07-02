#include "time.hpp"

namespace r2d2 {

Time::Time() {
    last = std::chrono::steady_clock::now();
    current = last;
    delta_time = 0.0;
}

void Time::update() {
    current = std::chrono::steady_clock::now();
    delta_time = std::chrono::duration<double>(current - last).count();
    last = current;
}

double Time::delta() const {
    return delta_time;
}

double Time::now() const {
    return std::chrono::duration<double>(current.time_since_epoch()).count();
}

}
