#include <iostream>

#include "engine/core/performance/performance_profile.hpp"
#include "engine/core/threads/job_queue.hpp"

#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "CHECK failed at " << __FILE__ << ':' << __LINE__ \
                      << ": " #condition << '\\n'; \
            return 1; \
        } \
    } while (false)

int main() {
    {
        const auto profile = r2d2::detect_performance_profile(
            60, 8, 16, 0.8f, 1920, 1080);

        CHECK(profile.recommended_fps == 60);
        CHECK(profile.render_scale > 0.0f && profile.render_scale <= 1.0f);
        CHECK(profile.quality == "medium");
        CHECK(profile.monitor_refresh_hz == 60);
    }

    {
        const auto low_profile = r2d2::detect_performance_profile(
            60, 2, 4, 0.1f, -1920, 1080);

        CHECK(low_profile.recommended_fps == 30);
        CHECK(low_profile.quality == "low");
        CHECK(low_profile.render_scale == 1.0f);
    }

    {
        const auto sanitized_profile = r2d2::detect_performance_profile(
            0, 0, 0, 2.0f, 100000, 100000);

        CHECK(sanitized_profile.monitor_refresh_hz == 30);
        CHECK(sanitized_profile.gpu_score == 1.0f);
        CHECK(sanitized_profile.render_scale == 0.85f);
    }

    {
        r2d2::ThreadSafeQueue<int> queue;
        CHECK(queue.push(42));

        int value = -1;
        CHECK(queue.try_pop(value));
        CHECK(value == 42);
        CHECK(!queue.try_pop(value));

        CHECK(queue.push(7));
        queue.close();

        CHECK(!queue.push(9));
        CHECK(queue.wait_pop(value));
        CHECK(value == 7);
        CHECK(!queue.wait_pop(value));
    }

    std::cout << "r2d2_performance_test: OK\n";
    return 0;
}
