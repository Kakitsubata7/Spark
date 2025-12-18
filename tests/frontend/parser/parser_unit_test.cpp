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

TEST(ExprParserTest, ValidCases) {
    // !x!
    // !(x!)
    {
        Location _;
        AST ast;
        EXPECT_VALID(ExprParser, "!x!",
            *ast.make<PrefixExpr>(_, _, PrefixOp::LogNot,
                ast.make<PostfixExpr>(_, _, PostfixOp::ForceUnwrap,
                    ast.make<Identifier>(_, _, "x"))));
    }

    // a + b * c
    // a + (b * c)
    {
        Location _;
        AST ast;
        EXPECT_VALID(ExprParser, "a + b * c",
            *ast.make<BinaryExpr>(_, _, InfixOp::Add,
                ast.make<Identifier>(_, _, "a"),
                ast.make<BinaryExpr>(_, _, InfixOp::Mul,
                    ast.make<Identifier>(_, _, "b"),
                    ast.make<Identifier>(_, _, "c"))));
    }

    // !a.b + c * -d.e? / f.g.h!
    // (!a.b) + ((c * (-(d.e?))) / (f.g.h!))
    {
        Location _;
        AST ast;

        EXPECT_VALID(ExprParser,
            "!a.b + c * -d.e? / f.g.h!",
            *ast.make<BinaryExpr>(_, _, InfixOp::Add,
                // !a.b
                ast.make<PrefixExpr>(_, _, PrefixOp::LogNot,
                    ast.make<BinaryExpr>(_, _, InfixOp::MemberAccess,
                        ast.make<Identifier>(_, _, "a"),
                        ast.make<Identifier>(_, _, "b"))),

                // c * -d.e? / f.g.h!
                ast.make<BinaryExpr>(_, _, InfixOp::Div,

                    // c * -d.e?
                    ast.make<BinaryExpr>(_, _, InfixOp::Mul,
                        ast.make<Identifier>(_, _, "c"),
                        ast.make<PrefixExpr>(_, _, PrefixOp::Neg,
                            ast.make<PostfixExpr>(_, _, PostfixOp::Optional,
                                ast.make<BinaryExpr>(_, _, InfixOp::MemberAccess,
                                    ast.make<Identifier>(_, _, "d"),
                                    ast.make<Identifier>(_, _, "e"))))),

                    // f.g.h!
                    ast.make<PostfixExpr>(_, _, PostfixOp::ForceUnwrap,
                        ast.make<BinaryExpr>(_, _, InfixOp::MemberAccess,
                            ast.make<BinaryExpr>(_, _, InfixOp::MemberAccess,
                                ast.make<Identifier>(_, _, "f"),
                                ast.make<Identifier>(_, _, "g")),
                            ast.make<Identifier>(_, _, "h"))))));
    }

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
    // TODO: EXPECT_VALID(LiteralParser, "3.14", RealLiteral({}, {}, BigReal("3.14")));
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
