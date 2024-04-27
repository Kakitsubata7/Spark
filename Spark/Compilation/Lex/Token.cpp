#include "Token.hpp"

namespace Spark {

    /* ===== Operator ===== */

    std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << "[" << token.type << ": \"" << token.content << "\"]";
        return os;
    }

} // Spark
