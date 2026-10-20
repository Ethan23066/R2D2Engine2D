#pragma once

#include <atomic>
#include <mutex>
#include <string>

namespace r2d2 {

class System {
public:
    static void init();
    static void shutdown();
    static bool is_initialized();

private:
    static std::atomic<bool>& initialized_flag();
    static std::mutex& initialized_mutex();
};

} // namespace r2d2
