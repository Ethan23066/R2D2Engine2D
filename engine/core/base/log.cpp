#include "log.hpp"

#include <iostream>
#include <mutex>

namespace r2d2 {
namespace {
std::mutex& log_mutex() {
    static std::mutex mutex;
    return mutex;
}

const char* safe_message(const char* msg) noexcept {
    return msg ? msg : "";
}
} // namespace

void Log::info(const char* msg) {
    std::lock_guard lock(log_mutex());
    std::cout << "[INFO] " << safe_message(msg) << '\n';
}

void Log::warn(const char* msg) {
    std::lock_guard lock(log_mutex());
    std::cout << "[WARN] " << safe_message(msg) << '\n';
}

void Log::error(const char* msg) {
    std::lock_guard lock(log_mutex());
    std::cerr << "[ERROR] " << safe_message(msg) << '\n';
}

} // namespace r2d2
