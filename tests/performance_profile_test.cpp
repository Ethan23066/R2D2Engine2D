#include <cassert>
#include <iostream>

#include "engine/core/performance/performance_profile.hpp"
#include "engine/core/threads/job_queue.hpp"

int main() {
    {
        const auto profile = r2d2::detect_performance_profile(
            60,
            8,
            16,
            0.8f,
            1920,
            1080
        );

        assert(profile.recommended_fps >= 60);
        assert(profile.render_scale > 0.0f && profile.render_scale <= 1.0f);
        assert(profile.quality == "high" || profile.quality == "medium");
    }

    {
        r2d2::ThreadSafeQueue<int> queue;
        queue.push(42);

        int value = -1;
        assert(queue.try_pop(value));
        assert(value == 42);
    }

    std::cout << "r2d2_performance_test: OK\n";
    return 0;
}
