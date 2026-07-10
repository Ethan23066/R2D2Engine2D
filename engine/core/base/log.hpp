#pragma once
#include <string>
#include <iostream>

namespace r2d2 {

class Log {
public:
    static void info(const char* msg);
    static void warn(const char* msg);
    static void error(const char* msg);
};

}
