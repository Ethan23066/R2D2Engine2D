#include "system.hpp"
#include "log.hpp"

namespace r2d2 {

static bool initialized = false;

void System::init() {
    if (!initialized) {
        Log::info("System initialized");
        initialized = true;
    }
}

void System::shutdown() {
    if (initialized) {
        Log::info("System shutdown");
        initialized = false;
    }
}

bool System::is_initialized() {
    return initialized;
}

}
