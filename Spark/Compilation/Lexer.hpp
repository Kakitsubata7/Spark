#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace Spark {

class Lexer {

    /* ===== Constructor ===== */

public:
    Lexer() = default;



    /* ===== Data ===== */

private:
    std::string current;

    bool isLineCommenting = false;
    bool isGroupCommenting = false;



    /* ===== Operations ====== */

public:
    void append(char c, const std::optional<char>& next);



    /* ===== Checks ===== */

private:
    static bool isKeyword(const std::string& str) {

        static const std::unordered_set<std::string> keywordSet = {
            "function",
            "if", "else",
            "for", "while", "do"
        };

        return keywordSet.find(str) != keywordSet.end();
    }

};

} // Spark
