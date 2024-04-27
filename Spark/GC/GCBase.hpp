#pragma once

namespace Spark {

class GCNode;

class GCBase {

    /* ===== Operations ===== */

public:
    virtual void addEntryNode(GCNode* node) = 0;
    virtual void removeEntryNode(GCNode* node) = 0;

    virtual void step() = 0;

};

} // Spark
