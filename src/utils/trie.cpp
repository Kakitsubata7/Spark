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
            continue;
        }
        // Otherwise, insert a new node
        node = &(*(node->children.insert(it, Node(c))));
    }
}

Trie::Match Trie::match(std::string_view s) const {
    if (s.empty()) {
        // Empty string is a part of any entry
        return root.children.empty() ? Match::None : Match::Partial;
    }

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

    // Match::Final if no further possible matches, Match::Partial otherwise
    return node->children.empty() ? Match::Final : Match::Partial;
}

} // spark
