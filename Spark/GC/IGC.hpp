#pragma once

#include "GCNode.hpp"

namespace Spark {

class IGC {

public:
    virtual GCNode* allocate() = 0;
    virtual void collect() = 0;

};

} // Spark
