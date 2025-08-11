#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "runtime/drc.hpp"

using ::testing::UnorderedElementsAreArray;

using Spark::Runtime::DRC;
using Spark::Runtime::DRCNode;
using Spark::Runtime::DRCHeader;
using Spark::Runtime::RCInt;

DRCHeader newObj(RCInt externalRefCount) {
    return DRCHeader { .node = nullptr, .rcHeader = { .refCount = externalRefCount, .type = nullptr } };
}

TEST(DRCTest, Add) {
    DRC drc;

    DRCHeader objA = newObj(0);
    DRCNode* a = drc.add(&objA);
    EXPECT_EQ(drc.nodeCount(), 1);
    EXPECT_EQ(a->obj, &objA);

    DRCHeader objB = newObj(0);
    DRCNode* b = drc.add(&objB);
    EXPECT_EQ(drc.nodeCount(), 2);
    EXPECT_EQ(b->obj, &objB);
}

TEST(DRCTest, RetainSingleExternallyReferencedNode) {
    // One node gets collected if it's not externally referenced
    DRC drc;
    DRCHeader obj = newObj(0);
    DRCNode* node = drc.add(&obj);
    EXPECT_EQ(drc.tryCleanup(node).size(), 1);
}

TEST(DRCTest, RetainNonCyclic) {
    // Non-cyclic reference gets collected
    DRC drc;
    DRCHeader objA = newObj(0);
    DRCHeader objB = newObj(0);
    DRCNode* a = drc.add(&objA);
    DRCNode* b = drc.add(&objB);
    drc.retain(a, b); // a -> b
    EXPECT_EQ(drc.tryCleanup(a).size(), 2);
}

TEST(DRCTest, RetainInternallyCyclic) {
    // Internal cyclic gets collected
    DRC drc;
    DRCHeader objA = newObj(0);
    DRCHeader objB = newObj(0);
    DRCNode* a = drc.add(&objA);
    DRCNode* b = drc.add(&objB);
    drc.retain(a, b); // a -> b
    drc.retain(b, a); // a <-> b
    EXPECT_EQ(drc.tryCleanup(a).size(), 2);
}

TEST(DRCTest, RetainExternallyReferenced) {
    // Externally referenced nodes can't be collected
    DRC drc;
    DRCHeader objA = newObj(1);
    DRCHeader objB = newObj(0);
    DRCNode* a = drc.add(&objA);
    DRCNode* b = drc.add(&objB);
    drc.retain(a, b); // a -> b
    drc.retain(b, a); // a <-> b
    EXPECT_TRUE(drc.tryCleanup(a).empty());
}

TEST(DRCTest, RetainTrioCycle) {
    // [a] -> b -> c -> a gets collected
    DRC drc;
    DRCHeader objA = newObj(0); DRCNode* a = drc.add(&objA);
    DRCHeader objB = newObj(0); DRCNode* b = drc.add(&objB);
    DRCHeader objC = newObj(0); DRCNode* c = drc.add(&objC);
    drc.retain(a, b); // a -> b
    drc.retain(b, c); // a -> b -> c
    drc.retain(c, a); // a -> b -> c -> a
    EXPECT_EQ(drc.tryCleanup(a).size(), 3);
}

TEST(DRCTest, RetainComplexCase1) {
    // [a] -> b, c
    // b -> c, d
    // c -> b
    // d -> a, f
    // e -> a
    // Can be collected
    DRC drc;
    DRCHeader objA = newObj(0); DRCNode* a = drc.add(&objA);
    DRCHeader objB = newObj(0); DRCNode* b = drc.add(&objB);
    DRCHeader objC = newObj(0); DRCNode* c = drc.add(&objC);
    DRCHeader objD = newObj(0); DRCNode* d = drc.add(&objD);
    DRCHeader objE = newObj(0); DRCNode* e = drc.add(&objE);
    DRCHeader objF = newObj(0); DRCNode* f = drc.add(&objF);

    drc.retain(a, b); drc.retain(a, c); drc.retain(a, e); // a -> b, c, e
    drc.retain(b, c); drc.retain(b, d); // b -> c, d
    drc.retain(c, b); // c -> b
    drc.retain(d, a); drc.retain(d, f); // d -> a, f

    std::vector<DRCNode*> expected = { a, b, c, d, e, f };
    auto& actual = drc.tryCleanup(a);
    EXPECT_THAT(actual, UnorderedElementsAreArray(expected));
}
