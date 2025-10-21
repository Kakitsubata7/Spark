#pragma once

#include <istream>
#include <string_view>
#include <vector>

#include "../result.hpp"
#include "token.hpp"

namespace Spark::FrontEnd {

class Lexer {
private:
    std::istream& _stream;

public:
    explicit Lexer(std::istream& stream) : _stream(stream) { }

    Result<Token> lex();

    Result<std::vector<Token>> lexAll();

#ifdef SPARK_TESTS_ENABLED
public:
#else
private:
#endif
    static Result<Token> lexWord(std::istream& stream, size_t& line, size_t& column);
    static Result<Token> lexDecimalNumber(std::istream& stream, size_t& line, size_t& column);
    static Result<Token> lexBinaryNumber(std::istream& stream, size_t& line, size_t& column);
    static Result<Token> lexOctalNumber(std::istream& stream, size_t& line, size_t& column);
    static Result<Token> lexHexNumber(std::istream& stream, size_t& line, size_t& column);
    static Result<Token> lexString(std::istream& stream, size_t& line, size_t& column);
    static Result<std::string> lexEscapeSequence(std::istream& stream, size_t& line, size_t& column);
    static Result<Token> lexPunctuator(std::istream& stream, size_t& line, size_t& column);
    static Result<Token> lexLineComment(std::istream& stream, size_t& line, size_t& column);
    static Result<Token> lexGroupComment(std::istream& stream, size_t& line, size_t& column);

    static Result<Token> lex(std::istream& stream, size_t& line, size_t& column);

    /**
     * Counts the line and column numbers at the current position, then returns them as a pair.
     * @param stream Input stream to count.
     * @return Line and column numbers as a pair.
     */
    static std::pair<size_t, size_t> countLineAndColumn(std::istream& stream);

    static bool streamStartWith(std::istream& stream, std::string_view sv);
    static bool streamStartWithIgnoreCase(std::istream& stream, std::string_view sv);
};

} // Spark::FrontEnd
