#include <gtest/gtest.h>

#include "frontend/ast/name.hpp"

using namespace Spark::FrontEnd;

TEST(NameValueTest, Equality) {
    EXPECT_EQ(NameValue::identifier("x"), NameValue::identifier("x"));
    EXPECT_NE(NameValue::identifier("x"), NameValue::identifier("y"));

    EXPECT_EQ(NameValue::discard(), NameValue::discard());
    EXPECT_NE(NameValue::discard(), NameValue::self());

    EXPECT_EQ(NameValue::op(OverOpKind::Add), NameValue::op(OverOpKind::Add));
    EXPECT_NE(NameValue::op(OverOpKind::Add), NameValue::op(OverOpKind::Sub));
}

TEST(NameValueInternerTest, Deduplication) {
    NameValueInterner interner;

    auto x1 = interner.intern(NameValue::identifier("x"));
    auto x2 = interner.intern(NameValue::identifier("x"));
    auto y  = interner.intern(NameValue::identifier("y"));

    EXPECT_EQ(x1, x2);
    EXPECT_NE(x1, y);
}

TEST(NameValueInternerTest, SpecialNames) {
    NameValueInterner interner;

    auto discard = interner.intern(NameValue::discard());
    auto self = interner.intern(NameValue::self());
    auto ctor = interner.intern(NameValue::constructor());
    auto dtor = interner.intern(NameValue::destructor());

    EXPECT_NE(discard, self);
    EXPECT_NE(self, ctor);
    EXPECT_NE(ctor, dtor);

    EXPECT_EQ(discard, interner.intern(NameValue::discard()));
    EXPECT_EQ(self, interner.intern(NameValue::self()));
    EXPECT_EQ(ctor, interner.intern(NameValue::constructor()));
    EXPECT_EQ(dtor, interner.intern(NameValue::destructor()));
}

TEST(NameValueInternerTest, OperatorDeduplication) {
    NameValueInterner interner;

    auto add1 = interner.intern(NameValue::op(OverOpKind::Add));
    auto add2 = interner.intern(NameValue::op(OverOpKind::Add));
    auto sub  = interner.intern(NameValue::op(OverOpKind::Sub));

    EXPECT_EQ(add1, add2);
    EXPECT_NE(add1, sub);
}

TEST(NameValueInternerTest, Mixed) {
    NameValueInterner interner;

    auto x = interner.intern(NameValue::identifier("x"));
    auto self = interner.intern(NameValue::self());
    auto add = interner.intern(NameValue::op(OverOpKind::Add));

    EXPECT_NE(x, self);
    EXPECT_NE(x, add);
    EXPECT_NE(self, add);
}

TEST(NameValueInternerTest, Value) {
    NameValueInterner interner;

    auto hello = interner.intern(NameValue::identifier("hello"));
    const NameValue& v = hello.value();
    EXPECT_TRUE(v.isIdentifier());
    EXPECT_EQ(v.str(), "hello");

    auto hello2 = interner.intern(NameValue::identifier("hello"));
    EXPECT_EQ(&hello.value(), &hello2.value());
}

TEST(NameValueInternerTest, DedupStressTest) {
    NameValueInterner interner;

    std::vector<InternedNameValue> vals;
    for (int i = 0; i < 1000; ++i) {
        vals.push_back(interner.intern(NameValue::identifier("x")));
    }
    for (auto& v : vals) {
        EXPECT_EQ(v, vals[0]);
    }
}
