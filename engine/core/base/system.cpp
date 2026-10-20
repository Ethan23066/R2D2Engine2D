#include "system.hpp"

#include "log.hpp"

namespace r2d2 {

std::atomic<bool>& System::initialized_flag() {
    static std::atomic<bool> initialized{false};
    return initialized;
}

std::mutex& System::initialized_mutex() {
    static std::mutex mutex;
    return mutex;
}

void System::init() {
    std::scoped_lock lock(initialized_mutex());
    if (!initialized_flag().load(std::memory_order_acquire)) {
        Log::info("System initialized");
        initialized_flag().store(true, std::memory_order_release);
    }
}

void System::shutdown() {
    std::scoped_lock lock(initialized_mutex());
    if (initialized_flag().load(std::memory_order_acquire)) {
        Log::info("System shutdown");
        initialized_flag().store(false, std::memory_order_release);
    }
}

bool System::is_initialized() {
    return initialized_flag().load(std::memory_order_acquire);
}

} // namespace r2d2
