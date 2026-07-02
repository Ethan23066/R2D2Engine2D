#pragma once
#include <string>

namespace r2d2 {

class System {
public:
    static void init();
    static void shutdown();
    static bool is_initialized();
};

}
