#pragma once

#include <initializer_list>
#include <string_view>
#include <vector>

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
        std::vector<Node> children;

        explicit Node(char ch, bool isEnd) : ch(ch), children({}) { }
    };

public:
    enum class Match {
        None    = -1,
        Final   = 0,
        Partial = 1
    };



    /* ===== Data ===== */

private:
    Node root;



    /* ===== Constructors ===== */

public:
    Trie() : root('\0', false) { }

    template <typename It>
    Trie(It first, It last) : root('\0', false) {
        for (It curr = first; curr != last; ++curr) {
            append(*curr);
        }
    }

    Trie(std::initializer_list<std::string_view> init) : Trie(init.begin(), init.end()) { }



    /* ===== Operations ===== */

private:
    void append(std::string_view s);

public:
    [[nodiscard]] Match match(std::string_view s) const;

};

} // spark
