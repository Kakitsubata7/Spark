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



    /* ===== Tokenization ===== */

private:
    static std::unordered_set<std::string> keywordSet;
    static std::unordered_set<std::string> operatorSet;
    static std::unordered_set<char> delimiterSet;

    static bool isKeyword(const std::string& str) {
        return keywordSet.find(str) != keywordSet.end();
    }

    static bool isOperator(const std::string& str) {
        return operatorSet.find(str) != operatorSet.end();
    }

    static bool isOperator(char c) {
        return isOperator(std::string(1, c));
    }

    static bool isNumericalLiteral(const std::string& str) {
        std::istringstream iss(str);
        double num;
        iss >> num;
        return iss.eof() && !iss.fail();
    }



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
