#pragma once

#include <unordered_set>
#include <vector>

#include "GCHandle.hpp"
#include "GCNode.hpp"

namespace Spark {

    class GC {

    private:
        std::unordered_set<GCNode*> nodes;
        std::vector<GCHandle*> roots;

    };

} // Spark
