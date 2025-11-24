#pragma once

#include <algorithm>
#include <string>

namespace Spark::Test::FrontEnd {

inline void removeCarriageReturns(std::string& s) {
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
}

} // Spark::Test::FrontEnd
