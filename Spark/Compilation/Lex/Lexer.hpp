#pragma once

#include <sstream>
#include <unordered_set>
#include <vector>

#include "Token.hpp"

namespace Spark {

class Lexer {

    /* ===== Data ===== */

private:
    std::istringstream iss;

    enum class State {
        None,
        LineComment,
        GroupComment,
        String,
        RawString,
        FormattedString
    };

    State state = State::None;
    char stringEndChar = 0;



    /* ===== Tokenization ===== */

private:
    static std::unordered_set<std::string> keywordSet;
    static std::unordered_set<std::string> operatorSet;
    static std::unordered_set<char> delimiterSet;

    static bool isKeyword(const std::string& str);
    static bool isOperator(const std::string& str);
    static bool isOperator(char c);
    static bool isNumericalLiteral(const std::string& str);

    /**
     * First character can be letters, '_', or '@'. The rest can be letters, digits, or '_'.
     */
    static bool isIdentifier(const std::string& str);



    /* ===== Constructor ===== */

public:
    explicit Lexer(const std::string& str) : iss(str) { }



    /* ===== Operations ===== */

public:
    [[nodiscard]]
    static std::vector<Token> lex(const std::string& str);

    [[nodiscard]]
    std::vector<Token> lex();

};

} // Spark
