#include "trie.hpp"

#include <algorithm>

namespace spark {

/* ===== Operation ===== */

Trie::Match Trie::match(std::string_view s) const {
    Node& root = *(this->root);
    Node& node = root;
    for (char c : s) {
        auto it = std::lower_bound(node.children.begin(), node.children.end(), c,
                                                     [](const std::unique_ptr<Node>& lhs, char rhs) {
                                                         return lhs->ch < rhs;
                                                     });
        // If current character exists, proceed to the next level
        if (it != node.children.end() && it->get()->ch == c) {
            node = **it;
            continue;
        }
        // Otherwise, no match
        return Match::NotFound;
    }

    if (!node.isEnd) {
        // Not an entry ending, no match
        return Match::NotFound;
    }
    return node.children.empty() ? Match::Complete : Match::Prefix;
}

} // spark
