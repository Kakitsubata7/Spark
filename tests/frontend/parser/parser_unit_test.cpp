#include <gtest/gtest.h>

#include "frontend/ast.hpp"
#include "frontend/lexer.hpp"
#include "frontend/parser/parser_unit.hpp"
#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"
#include "utils/error.hpp"
#include "utils/result.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

#define EXPECT_VALID(ParserUnitType, source, expectedNode)    \
{                                                             \
    Lexer lexer(source);                                      \
    SimpleTokenProducer producer(lexer);                      \
    AST ast;                                                  \
    ParserUnitType parser(producer, ast);                     \
    Result<Node*, Error> result = parser.parse();             \
    EXPECT_TRUE(result.hasValue()) << result.error().message; \
    EXPECT_EQ(*result.value(), (expectedNode));               \
}

#define EXPECT_ERROR(ParserUnitType, source)      \
{                                                 \
    Lexer lexer(source);                          \
    SimpleTokenProducer producer(lexer);          \
    AST ast;                                      \
    ParserUnitType parser(producer, ast);         \
    Result<Node*, Error> result = parser.parse(); \
    EXPECT_TRUE(result.hasError());               \
}

TEST(BindingPatternTest, InvalidCases) {
    EXPECT_ERROR(BindingPatternParser, "(x)");
    EXPECT_ERROR(BindingPatternParser, "1");
    EXPECT_ERROR(BindingPatternParser, "(x): T");
    // TODO: EXPECT_ERROR(BindingPatternParser, "x:");
    EXPECT_ERROR(BindingPatternParser, "1: T");
}

TEST(QualifiedNameParserTest, ValidCases) {
    EXPECT_VALID(QualifiedNameParser, "foo", Identifier({}, {}, "foo"));
    EXPECT_VALID(QualifiedNameParser, "bar", Identifier({}, {}, "bar"));
    EXPECT_VALID(QualifiedNameParser, "_", Discard({}, {}));
    EXPECT_VALID(QualifiedNameParser, "x", Identifier({}, {}, "x"));
    EXPECT_VALID(QualifiedNameParser, "__", Identifier({}, {}, "__"));
}

TEST(QualifiedNameParserTest, InvalidCases) {
    EXPECT_ERROR(QualifiedNameParser, "1");
    EXPECT_ERROR(QualifiedNameParser, "(foo)");
    EXPECT_ERROR(QualifiedNameParser, ".bar");
}

TEST(LiteralParserTest, ValidCases) {
    EXPECT_VALID(LiteralParser, "37", IntLiteral({}, {}, BigInt(37)));
    // EXPECT_VALID(LiteralParser, "3.14", RealLiteral({}, {}, BigReal("3.14")));
    EXPECT_VALID(LiteralParser, "true", BoolLiteral({}, {}, true));
    EXPECT_VALID(LiteralParser, "false", BoolLiteral({}, {}, false));
    EXPECT_VALID(LiteralParser, "'foo'", StringLiteral({}, {}, "foo"));
    EXPECT_VALID(LiteralParser, "nil", NilLiteral({}, {}));
    EXPECT_VALID(LiteralParser, "()", VoidLiteral({}, {}));
    EXPECT_VALID(LiteralParser, "( )", VoidLiteral({}, {}));
    EXPECT_VALID(LiteralParser, "(     )", VoidLiteral({}, {}));
    EXPECT_VALID(LiteralParser, "undefined", Undefined({}, {}));
}

TEST(LiteralParserTest, InvalidCases) {
    EXPECT_ERROR(LiteralParser, "null");
    EXPECT_ERROR(LiteralParser, "x");
    EXPECT_ERROR(LiteralParser, "(1)");
}
