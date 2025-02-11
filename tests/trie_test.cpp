#include <gtest/gtest.h>

#include "utils/trie.hpp"

using namespace spark;

using namespace spark;

class TrieTest : public ::testing::Test {
protected:
    Trie trie;

    void SetUp() override {
        trie = Trie({"apple", "app", "banana", "bat", "batman"});
    }
};

TEST_F(TrieTest, MatchExactWordWithNoFurtherMatches) {
    EXPECT_EQ(trie.match("banana"), Trie::Match::Final);  // Exact match, no further words
}

TEST_F(TrieTest, MatchExactWordWithFurtherMatches) {
    EXPECT_EQ(trie.match("app"), Trie::Match::Partial);  // Exact match but has "apple" as a further match
    EXPECT_EQ(trie.match("bat"), Trie::Match::Partial);  // Exact match but has "batman" as a further match
}

TEST_F(TrieTest, MatchPrefixWithFurtherMatches) {
    EXPECT_EQ(trie.match("appl"), Trie::Match::Partial); // Prefix of "apple"
    EXPECT_EQ(trie.match("ban"), Trie::Match::Partial);  // Prefix of "banana"
    EXPECT_EQ(trie.match("batm"), Trie::Match::Partial); // Prefix of "batman"
}

TEST_F(TrieTest, NoMatch) {
    EXPECT_EQ(trie.match("pear"), Trie::Match::None);        // Not in the trie
    EXPECT_EQ(trie.match("batmobile"), Trie::Match::None);   // Not a valid prefix
    EXPECT_EQ(trie.match("xyz"), Trie::Match::None);         // Completely unrelated
}

TEST_F(TrieTest, EmptyString) {
    EXPECT_EQ(trie.match(""), Trie::Match::Partial);  // "" is a part of any entry
}

TEST(TrieStandaloneTest, ConstructEmptyTrie) {
    Trie emptyTrie;
    EXPECT_EQ(emptyTrie.match(""), Trie::Match::None);
    EXPECT_EQ(emptyTrie.match("anything"), Trie::Match::None);
}

TEST(TrieStandaloneTest, ConstructWithEmptyString) {
    Trie trie({""});
    EXPECT_EQ(trie.match(""), Trie::Match::None);  // "" entry is ignored
}

TEST(TrieStandaloneTest, ConstructFromInitializerList) {
    Trie trie({"hello", "world"});
    EXPECT_EQ(trie.match("hello"), Trie::Match::Final);
    EXPECT_EQ(trie.match("worl"), Trie::Match::Partial);
    EXPECT_EQ(trie.match("worlds"), Trie::Match::None);
}
