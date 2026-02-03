#pragma once

#include "frontend/ast/name.hpp"
#include "utils/location.hpp"

namespace Spark::FrontEnd {

struct Symbol {
    InternedNameValue name;
    Location defStart;
    Location defEnd;

    bool isReassignable;
    bool isReference;
};

} // Spark::FrontEnd
