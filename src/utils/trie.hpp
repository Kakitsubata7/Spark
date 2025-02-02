#pragma once

#include <string_view>
#include <vector>

#include <iostream>

namespace spark {

/**
 * This class is implemented as a read-only trie that supports matching @c std::string  while knowing if it is the
 * longest possible match.
 */
class Trie {

    /* ===== Types ==== */

private:
    struct Node {
        char ch;
        bool isEnd;
        std::vector<Node> children;

        explicit Node(char ch, bool isEnd) : ch(ch), isEnd(isEnd), children({}) { }
    };

public:
    enum class Match {
        NotFound,
        Prefix,
        Complete
    };



    /* ===== Data ===== */

private:
    Node root;



    /* ===== Constructors ===== */

public:
    template <typename It>
    Trie(It first, It last) : root('\0', false) {
        for (It curr = first; curr != last; ++curr) {
            append(*curr);
        }
    }

    Trie() : root('\0', false) { }



    /* ===== Operations ===== */

private:
    void append(std::string_view s);

public:
    [[nodiscard]] Match match(std::string_view s) const;

};

} // spark
