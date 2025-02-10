#include "trie.hpp"

#include <algorithm>

namespace spark {

/* ===== Operations ===== */

void Trie::append(std::string_view s) {
    Node* node = &root;
    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];

        auto it = std::lower_bound(node->children.begin(), node->children.end(), c,
            [](const Node& lhs, char rhs) { return lhs.ch < rhs; });
        // If current character exists, proceed to the next level
        if (it != node->children.end() && it->ch == c) {
            node = &(*it);
            if (i == s.length() - 1) {
                node->isEnd = true;  // Mark as complete word
            }
            continue;
        }
        // Otherwise, insert a new node
        node = &(*(node->children.insert(it, Node(c, i == s.length() - 1))));
    }
}

Trie::Match Trie::match(std::string_view s) const {
    const Node* node = &(this->root);
    for (char c : s) {
        auto it = std::lower_bound(node->children.cbegin(), node->children.cend(), c,
            [](const Node& lhs, char rhs) { return lhs.ch < rhs; });
        // If current character exists, proceed to the next level
        if (it != node->children.cend() && it->ch == c) {
            node = &(*it);
            continue;
        }
        // Otherwise, no match
        return Match::None;
    }

    if (!node->isEnd) {
        // Not an entry ending, no match
        return Match::None;
    }
    return node->children.empty() ? Match::Final : Match::Partial;
}

} // spark
