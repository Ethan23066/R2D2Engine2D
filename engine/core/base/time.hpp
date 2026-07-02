#pragma once
#include <chrono>

namespace r2d2 {

class Time {
public:
    Time();

    void update();
    double delta() const;
    double now() const;

private:
    std::chrono::steady_clock::time_point last;
    std::chrono::steady_clock::time_point current;
    double delta_time;
};

}
