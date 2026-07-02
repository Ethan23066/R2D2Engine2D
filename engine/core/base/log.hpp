#pragma once
#include <string>
#include <iostream>

namespace r2d2 {

class Log {
public:
    static void info(const std::string& msg);
    static void warn(const std::string& msg);
    static void error(const std::string& msg);
};

}
