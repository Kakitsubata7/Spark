#include <sstream>

#include <gtest/gtest.h>

#include "frontend/lexer.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

using TT = TokenType;

static void lexTest(std::string_view source, const std::vector<TT>& expectedTypes) {
    std::string s = std::string{source};
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    std::istringstream iss{s};
    auto [actualToks, _, diagnostics] = Lexer::lexAll(iss);

    EXPECT_EQ(actualToks.size(), expectedTypes.size()) << "Number of tokens mismatched\n";
    for (size_t i = 0; i < actualToks.size(); ++i) {
        if (actualToks[i].type != expectedTypes[i]) {
            std::ostringstream oss;
            oss << "Token type mismatch at index " << i << "\n"
                << "Expected: " << expectedTypes[i] << "\n"
                << "Actual:   " << actualToks[i].type << "\n\n";
            FAIL() << oss.str();
        }
    }

    if (diagnostics.hasError()) {
        std::ostringstream oss;
        oss << "\nErrors: \n";
        for (const Diagnostic& d : diagnostics.diagnostics()) {
            oss << d << "\n";
        }
        FAIL() << oss.str();
    }
}

TEST(LexTest, Example1) {
    std::string_view source = R"(
fn potsOfGold(pots: Array<Int>^) -> Int do
    fn recursion(pots: Array<Int>^, i: Int, j: Int, opt: Array<Array<Int>^>^) -> Int do
        if i > j do
            return 0
        end

        if opt[i][j] != 0 do
            return opt[i][j]
        end

        if i == j do
            opt[i][j] = pots[i]
            return opt[i][j]
        end

        const pickLeft = pots[i] + min(
            recursion(pots, i + 2, j, opt),
            recursion(pots, i + 1, j - 1, opt)
        )

        const pickRight = pots[j] + min(
            recursion(pots, i, j - 2, opt),
            recursion(pots, i + 1, j - 1, opt)
        )

        opt[i][j] = max(pickLeft, pickRight)
        return opt[i][j]
    end

    const n = pots.size()
    const opt: Array<Array<Int>^>^ = Array(n)
    for i in 0...n do
        opt[i] = Array(n)
        for j in 0...n do
            opt[i][j] = 0
        end
    end

    return recursion(pots, 0, n - 1, opt)
end

print(potsOfGold({10, 5, 15, 20}))
)";
    std::vector<TokenType> expectedTypes = {
        TT::Fn, TT::Identifier, TT::LParen, TT::Identifier, TT::Colon, TT::Identifier, TT::Lt, TT::Identifier, TT::Gt, TT::Caret, TT::RParen, TT::Arrow, TT::Identifier, TT::Do,
            TT::Fn, TT::Identifier, TT::LParen, TT::Identifier, TT::Colon, TT::Identifier, TT::Lt, TT::Identifier, TT::Gt, TT::Caret, TT::Comma, TT::Identifier, TT::Colon, TT::Identifier, TT::Comma, TT::Identifier, TT::Colon, TT::Identifier, TT::Comma, TT::Identifier, TT::Colon, TT::Identifier, TT::Lt, TT::Identifier, TT::Lt, TT::Identifier, TT::Gt, TT::Caret, TT::Gt, TT::Caret, TT::RParen, TT::Arrow, TT::Identifier, TT::Do,
                TT::If, TT::Identifier, TT::Gt, TT::Identifier, TT::Do,
                    TT::Return, TT::Integer,
                TT::End,
                TT::If, TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::LBracket, TT::Identifier, TT::RBracket, TT::Ne, TT::Integer, TT::Do,
                    TT::Return, TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::LBracket, TT::Identifier, TT::RBracket,
                TT::End,
                TT::If, TT::Identifier, TT::Eq, TT::Identifier, TT::Do, // 81
                    TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::LBracket, TT::Identifier, TT::RBracket, TT::Assign, TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket,
                    TT::Return, TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::LBracket, TT::Identifier, TT::RBracket,
                TT::End,
                TT::Const, TT::Identifier, TT::Assign, TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::Add, TT::Identifier, TT::LParen,
                    TT::Identifier, TT::LParen, TT::Identifier, TT::Comma, TT::Identifier, TT::Add, TT::Integer, TT::Comma, TT::Identifier, TT::Comma, TT::Identifier, TT::RParen, TT::Comma,
                    TT::Identifier, TT::LParen, TT::Identifier, TT::Comma, TT::Identifier, TT::Add, TT::Integer, TT::Comma, TT::Identifier, TT::Sub, TT::Integer, TT::Comma, TT::Identifier, TT::RParen,
                TT::RParen,
            TT::Const, TT::Identifier, TT::Assign, TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::Add, TT::Identifier, TT::LParen,
                TT::Identifier, TT::LParen, TT::Identifier, TT::Comma, TT::Identifier, TT::Comma, TT::Identifier, TT::Sub, TT::Integer, TT::Comma, TT::Identifier, TT::RParen, TT::Comma,
                TT::Identifier, TT::LParen, TT::Identifier, TT::Comma, TT::Identifier, TT::Add, TT::Integer, TT::Comma, TT::Identifier, TT::Sub, TT::Integer, TT::Comma, TT::Identifier, TT::RParen,
            TT::RParen,
            TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::LBracket, TT::Identifier, TT::RBracket, TT::Assign, TT::Identifier, TT::LParen, TT::Identifier, TT::Comma, TT::Identifier, TT::RParen,
            TT::Return, TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::LBracket, TT::Identifier, TT::RBracket,
        TT::End,

            TT::Const, TT::Identifier, TT::Assign, TT::Identifier, TT::Dot, TT::Identifier, TT::LParen, TT::RParen,
            TT::Const, TT::Identifier, TT::Colon, TT::Identifier, TT::Lt, TT::Identifier, TT::Lt, TT::Identifier, TT::Gt, TT::Caret, TT::Gt, TT::Caret, TT::Assign, TT::Identifier, TT::LParen, TT::Identifier, TT::RParen,
            TT::For, TT::Identifier, TT::In, TT::Integer, TT::Range, TT::Identifier, TT::Do,
                TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::Assign, TT::Identifier, TT::LParen, TT::Identifier, TT::RParen,
                TT::For, TT::Identifier, TT::In, TT::Integer, TT::Range, TT::Identifier, TT::Do,
                    TT::Identifier, TT::LBracket, TT::Identifier, TT::RBracket, TT::LBracket, TT::Identifier, TT::RBracket, TT::Assign, TT::Integer,
                TT::End,
            TT::End,
            TT::Return, TT::Identifier, TT::LParen, TT::Identifier, TT::Comma, TT::Integer, TT::Comma, TT::Identifier, TT::Sub, TT::Integer, TT::Comma, TT::Identifier, TT::RParen,
        TT::End,

        TT::Identifier, TT::LParen, TT::Identifier, TT::LParen, TT::LBrace, TT::Integer, TT::Comma, TT::Integer, TT::Comma, TT::Integer, TT::Comma, TT::Integer, TT::RBrace, TT::RParen, TT::RParen
    };
    lexTest(source, expectedTypes);
}
