#pragma once

namespace Spark {

class GCNode;

class GCBase {

    /* ===== Entry Nodes ===== */

public:
    virtual void addEntryNode(GCNode* node) = 0;
    virtual void removeEntryNode(GCNode* node) = 0;

};

} // Spark
