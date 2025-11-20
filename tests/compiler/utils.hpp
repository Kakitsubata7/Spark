#pragma once

#include <algorithm>
#include <string>

namespace Spark::Test::Compiler {

inline void removeCarriageReturns(std::string& s) {
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
}

} // Spark::Test::Compiler
