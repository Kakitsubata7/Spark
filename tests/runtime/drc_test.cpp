#include <gtest/gtest.h>

#include "runtime/drc.hpp"

using Spark::Runtime::DRC;
using Spark::Runtime::DRCNode;
using Spark::Runtime::DRCHeader;

DRCHeader newObj() {
    return DRCHeader { .node = nullptr, .rcHeader = { .refCount = 0, .type = nullptr } };
}

TEST(DRCTest, Add) {
    DRC drc;

    DRCHeader objA = newObj();
    DRCNode* a = drc.add(&objA);
    EXPECT_EQ(drc.nodeCount(), 1);
    EXPECT_EQ(a->obj, &objA);

    DRCHeader objB = newObj();
    DRCNode* b = drc.add(&objB);
    EXPECT_EQ(drc.nodeCount(), 2);
    EXPECT_EQ(b->obj, &objB);
}

TEST(DRCTest, RetainNonCyclic) {
    // Non-cyclic reference gets collected
    DRC drc;
    DRCHeader objA = newObj();
    DRCHeader objB = newObj();
    DRCNode* a = drc.add(&objA);
    DRCNode* b = drc.add(&objB);
    drc.retain(a, b); // a -> b
    auto& collects = drc.tryCleanup(a);
    EXPECT_EQ(collects.size(), 2);
}

TEST(DRCTest, RetainInternallyCyclic) {
    // Internal cyclic gets collected
    DRC drc;
    DRCHeader objA = newObj();
    DRCHeader objB = newObj();
    DRCNode* a = drc.add(&objA);
    DRCNode* b = drc.add(&objB);
    drc.retain(a, b); // a -> b
    drc.retain(b, a); // a <-> b
    auto& collects = drc.tryCleanup(a);
    EXPECT_EQ(collects.size(), 2);
}
