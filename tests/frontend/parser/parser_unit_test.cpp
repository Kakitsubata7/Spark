#include <gtest/gtest.h>

#include "frontend/ast.hpp"
#include "frontend/lexer.hpp"
#include "frontend/parser/parser_unit.hpp"
#include "utils/error.hpp"
#include "utils/result.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

#define EXPECT_ERROR(ParserUnitType, source)      \
{                                                 \
    Lexer lexer(source);                          \
    SimpleTokenProducer producer(lexer);          \
    AST ast;                                      \
    ParserUnitType parser(producer, ast);         \
    Result<Node*, Error> result = parser.parse(); \
    EXPECT_TRUE(result.hasError());               \
}

TEST(BindingPatternTest, Invalid1) {
    EXPECT_ERROR(BindingPatternParser, "(x)");
    EXPECT_ERROR(BindingPatternParser, "1");
    EXPECT_ERROR(BindingPatternParser, "(x): T");
    EXPECT_ERROR(BindingPatternParser, "x:");
    EXPECT_ERROR(BindingPatternParser, "1: T");
}
