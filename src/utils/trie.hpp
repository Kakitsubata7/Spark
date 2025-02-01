#pragma once

#include <memory>
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
        bool isEnd;
        std::vector<std::unique_ptr<Node>> children;

        explicit Node(char ch, bool isEnd) : ch(ch), isEnd(isEnd) { }
    };

public:
    enum class Match {
        NotFound,
        Prefix,
        Complete
    };



    /* ===== Data ===== */

private:
    std::unique_ptr<Node> root;



    /* ===== Constructor ===== */

public:
    template <typename It>
    Trie(It first, It last) {
        // TODO: Build trie
    }

    Trie() : root(std::make_unique<Node>('\0', false)) { }



    /* ===== Operation ===== */

public:
    [[nodiscard]] Match match(std::string_view s) const;

};

} // spark
