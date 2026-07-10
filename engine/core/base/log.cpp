#include "log.hpp"
#include <string>
#include <iostream>

namespace r2d2 {

void Log::info(const char* msg) {
    std::cout << "[INFO] " << std::string(msg) << std::endl;
}

void Log::warn(const char* msg) {
    std::cout << "[WARN] " << std::string(msg) << std::endl;
}

void Log::error(const char* msg) {
    std::cerr << "[ERROR] " << std::string(msg) << std::endl;
}

}
