#include "log.hpp"

namespace r2d2 {

void Log::info(const std::string& msg) {
    std::cout << "[INFO] " << msg << std::endl;
}

void Log::warn(const std::string& msg) {
    std::cout << "[WARN] " << msg << std::endl;
}

void Log::error(const std::string& msg) {
    std::cerr << "[ERROR] " << msg << std::endl;
}

}
