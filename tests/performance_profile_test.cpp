#include <chrono>
#include <iostream>
#include <thread>

#include "engine/core/performance/performance_profile.hpp"
#include "engine/core/threads/job_queue.hpp"
#include "engine/core/window/window.hpp"

#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "CHECK failed at " << __FILE__ << ':' << __LINE__ \
                      << ": " #condition << '\n'; \
            return 1; \
        } \
    } while (false)

int main() {
    // 1. Test du micro-benchmark sur une charge rapide (doit viser la fréquence max de l'écran)
    {
        int calls = 0;
        const auto fast_res = r2d2::run_frame_benchmark(
            [&calls] { ++calls; },
            120, // monitor_refresh_hz
            2,   // warmup_frames
            10   // sample_frames
        );
        CHECK(calls == 12);
        CHECK(fast_res.avg_frame_ms >= 0.0);
        CHECK(fast_res.p95_frame_ms >= fast_res.avg_frame_ms);
        CHECK(fast_res.recommended_fps == 120);
        CHECK(fast_res.render_scale == 1.0f);
    }

    // 2. Test du micro-benchmark sur une charge lente (~25 ms par frame -> ~40 FPS -> bridé à 30 FPS)
    {
        const auto slow_res = r2d2::run_frame_benchmark(
            [] { std::this_thread::sleep_for(std::chrono::milliseconds(25)); },
            60, // monitor_refresh_hz
            1,  // warmup_frames
            5   // sample_frames
        );
        CHECK(slow_res.avg_frame_ms >= 20.0);
        CHECK(slow_res.recommended_fps == 30);
        CHECK(slow_res.render_scale == 0.85f);
    }

    // 3. Test de la file ThreadSafeQueue avec std::optional, close() et reset()
    {
        r2d2::ThreadSafeQueue<int> queue;
        CHECK(queue.empty());
        CHECK(queue.push(42));
        CHECK(queue.size() == 1);

        auto first = queue.try_pop();
        CHECK(first.has_value());
        CHECK(*first == 42);
        CHECK(!queue.try_pop().has_value());

        CHECK(queue.push(7));
        queue.close();
        CHECK(queue.is_closed());
        CHECK(!queue.push(9));

        auto remaining = queue.wait_pop();
        CHECK(remaining.has_value());
        CHECK(*remaining == 7);
        CHECK(!queue.wait_pop().has_value());

        queue.reset();
        CHECK(!queue.is_closed());
        CHECK(queue.empty());
        CHECK(queue.push(100));
        auto after_reset = queue.wait_pop();
        CHECK(after_reset.has_value() && *after_reset == 100);
    }

    // 4. Test de capacité maximale optionnelle
    {
        r2d2::ThreadSafeQueue<int> bounded_queue(2);
        CHECK(bounded_queue.push(1));
        CHECK(bounded_queue.push(2));
        CHECK(!bounded_queue.push(3));
        CHECK(bounded_queue.size() == 2);
    }

    // 5. Test producteur / consommateur multithread
    {
        r2d2::ThreadSafeQueue<int> queue;
        std::thread producer([&queue] { queue.push(99); });
        auto received = queue.wait_pop();
        producer.join();
        CHECK(received.has_value());
        CHECK(*received == 99);
    }

    // 6. Test de sélection du mode moniteur
    {
        const EngineMonitorMode modes[] = {
            {1280, 720, 60},
            {1920, 1080, 60},
            {1600, 900, 75}
        };
        CHECK(engine_window_select_monitor(modes, 3) == 1);
        CHECK(engine_window_select_monitor(nullptr, 3) == 0);
        CHECK(engine_window_select_monitor(modes, 0) == 0);
    }

    std::cout << "r2d2_performance_test: OK\n";
    return 0;
}
