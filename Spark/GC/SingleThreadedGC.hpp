#pragma once

#include "GCBase.hpp"

namespace Spark {

class SingleThreadedGC : public GCBase {

    /* ===== Operations ===== */

public:
    void addEntryNode(GCNode* node) override;
    void removeEntryNode(GCNode* node) override;

};

} // Spark
