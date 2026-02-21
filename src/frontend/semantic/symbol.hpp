#pragma once

#include "frontend/ast/node.hpp"

namespace Spark::FrontEnd {

struct Symbol {
    /**
     * Declaration `Name` node.
     */
    Name* name;

    /**
     * Whether the symbol is reassignable or not.
     */
    bool isReassignable;
};

} // Spark::FrontEnd
