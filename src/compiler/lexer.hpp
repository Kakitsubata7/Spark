#pragma once

#include <istream>
#include <sstream>
#include <string_view>
#include <vector>

#include "result.hpp"
#include "token.hpp"
#include "../utils/trie.hpp"

namespace spark {

class Lexer {

    /* ===== Syntax ===== */

public:
    using Match = Trie::Match;

    /**
     * Checks if a string is a keyword.
     * @param sv The string to check.
     * @return true if the string is a keyword, false otherwise.
     */
    static bool isKeyword(std::string_view sv);

    /**
     * Matches a string against all symbols, returns the result of the match.
     * @param sv The string to match.
     * @return Match::None if the string matches no symbol, and it's not a prefix of any symbols;
     * Match::Final if the string matches a symbol, and there's no further possible matches;
     * Match::Partial if the string is a prefix of any symbols (can but doesn't have to be an exact match of any symbols).
     */
    static Match matchSymbol(std::string_view sv);

    /**
     * Checks if a string is a valid identifier. A valid identifier must have an English alphabet or an underscore as
     * the first character, and English alphabets, digits, or underscores as the rest of the characters.
     * @param sv The string to check.
     * @return true if the string is a valid identifier, false otherwise.
     */
    static bool isIdentifier(std::string_view sv);



    /* ===== Data ===== */

private:
    const bool ignoreComments;



    /* ===== Constructor ===== */

public:
    explicit Lexer(bool ignoreComments = true) : ignoreComments(ignoreComments) { }



    /* ===== Operations ===== */

public:
    Result<std::vector<Token>> lex(std::istream& is);

    Result<std::vector<Token>> lex(const std::string& s) {
        std::istringstream iss(s);
        return lex(iss);
    }

};

} // spark
