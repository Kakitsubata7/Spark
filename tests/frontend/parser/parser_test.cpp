#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "frontend/ast.hpp"
#include "frontend/parser.hpp"
#include "utils/error.hpp"

#define MAKE(type, ...) ast.make<type>(Location(0, 0), Location(0, 0), __VA_ARGS__)

#define VARMOD(kind, isImmut, opt) MAKE(VarModifier, kind, isImmut, opt)
#define VARKIND VarModifier::VarKind
#define VAROPT VarModifier::Optionality
#define VARDEF(mod, pattern, ...) MAKE(VarDefStmt, mod, pattern, __VA_ARGS__)

#define ASSIGN(op, lhs, rhs) MAKE(AssignStmt, op, lhs, rhs)
#define ASSIGN_OP AssignStmt::OpKind

#define EXPR_STMT(expr) MAKE(ExprStmt, expr)

#define BLOCK(...) MAKE(BlockExpr, std::vector<Stmt*>{__VA_ARGS__})

#define BINARY(op, lhs, rhs) MAKE(BinaryExpr, op, lhs, rhs)
#define BINARY_OP BinaryExpr::OpKind

#define PREFIX(op, expr) MAKE(PrefixExpr, op, expr)
#define PREFIX_OP PrefixExpr::OpKind

#define IDENT(name) MAKE(IdentifierExpr, name)

using namespace Spark;
using namespace Spark::FrontEnd;

std::pair<AST, std::vector<Error>> parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    return Parser::parse(iss);
}

TEST(ParserTest, BlockTest) {
    auto [ast, errors] = parse("{ }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(BLOCK())
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest1) {
    // Making sure `*` binds tighter than `+`
    auto [ast, errors] = parse("a + b * c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BINARY(BINARY_OP::Add,
                IDENT("a"), BINARY(BINARY_OP::Mul, IDENT("b"), IDENT("c"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest2) {
    // Making sure `*` binds tighter than `+`
    auto [ast, errors] = parse("a * b + c * d");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BINARY(BINARY_OP::Add,
                BINARY(BINARY_OP::Mul, IDENT("a"), IDENT("b")),
                BINARY(BINARY_OP::Mul, IDENT("c"), IDENT("d")))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest3) {
    // Making sure arithmetic of the same operator is left-associative
    auto [ast, errors] = parse("a - b - c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BINARY(BINARY_OP::Sub,
                BINARY(BINARY_OP::Sub, IDENT("a"), IDENT("b")),
                IDENT("c")
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest4) {
    // Making sure prefix binds tighter than binary
    auto [ast, errors] = parse("-a * b");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BINARY(BINARY_OP::Mul,
                PREFIX(PREFIX_OP::Neg, IDENT("a")),
                IDENT("b")
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest5) {
    // Making sure prefix binds tighter than binary
    auto [ast, errors] = parse("-a + b");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BINARY(BinaryExpr::OpKind::Add,
               PREFIX(PREFIX_OP::Neg, IDENT("a")),
               IDENT("b")
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest6) {
    // Making sure expression in parentheses binds tighter than others
    auto [ast, errors] = parse("(a + b) * c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BINARY(BINARY_OP::Mul,
               BINARY(BINARY_OP::Add, IDENT("a"), IDENT("b")),
               IDENT("c")
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest7) {
    // Making sure comparison binds looser than arithmetic
    auto [ast, errors] = parse("a + b < c * d");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BINARY(BINARY_OP::Lt,
                BINARY(BINARY_OP::Add, IDENT("a"), IDENT("b")),
                BINARY(BINARY_OP::Mul, IDENT("c"), IDENT("d"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest8) {
    // Making sure `&&` binds tighter than `||`
    auto [ast, errors] = parse("a || b && c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BINARY(BINARY_OP::LogOr,
                IDENT("a"),
                BINARY(BINARY_OP::LogAnd, IDENT("b"), IDENT("c"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest9) {
    // Binary operator stress test
    auto [ast, errors] = parse("a + b * c < d && e || f");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BINARY(BINARY_OP::LogOr,
                BINARY(BINARY_OP::LogAnd,
                    BINARY(BINARY_OP::Lt,
                        BINARY(BINARY_OP::Add,
                            IDENT("a"),
                            BINARY(BINARY_OP::Mul, IDENT("b"), IDENT("c"))),
                        IDENT("d")),
                    IDENT("e")),
                IDENT("f")
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest10) {
    // Making sure comparison is non-associative
    auto [ast, errors] = parse("a < b < c");
    EXPECT_FALSE(errors.empty());
}

TEST(ParserTest, OperatorTest11) {
    // Making sure equality is non-associative
    auto [ast, errors] = parse("a == b === c");
    EXPECT_FALSE(errors.empty());
}

TEST(ParserTest, TypeofTest) {
    auto [ast, errors] = parse("typeof(x + y)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            MAKE(TypeofExpr,
                 BINARY(BINARY_OP::Add, IDENT("x"), IDENT("y"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, AssignTest1) {
    // Making sure `??=` binds looser than others
    auto [ast, errors] = parse("a = b ?\?= c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        ASSIGN(ASSIGN_OP::CoalesceAssign,
            ASSIGN(ASSIGN_OP::Assign, IDENT("a"), IDENT("b")),
            IDENT("c")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, AssignTest2) {
    // Making sure assignment is right-associative
    auto [ast, errors] = parse("a = b += c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        ASSIGN(ASSIGN_OP::Assign,
            IDENT("a"),
            ASSIGN(ASSIGN_OP::AddAssign, IDENT("b"), IDENT("c"))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, AssignTest3) {
    // Making sure assignment is right-associative
    auto [ast, errors] = parse("a = b += c *= d");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        ASSIGN(ASSIGN_OP::Assign,
            IDENT("a"),
            ASSIGN(ASSIGN_OP::AddAssign,
                IDENT("b"),
                ASSIGN(ASSIGN_OP::MulAssign, IDENT("c"), IDENT("d"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, VarDefTest1) {
    auto [ast, errors] = parse("let x: T = y");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        VARDEF(
            VARMOD(VARKIND::Let, false, VAROPT::None),
            MAKE(BindingPattern, "x"),
            IDENT("T"),
            IDENT("y")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, VarDefTest2) {
    auto [ast, errors] = parse("const^ x: T = y");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        VARDEF(
            VARMOD(VARKIND::Const, true, VAROPT::None),
            MAKE(BindingPattern, "x"),
            IDENT("T"),
            IDENT("y")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, VarDefTest3) {
    auto [ast, errors] = parse("let? x: T");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        VARDEF(
            VARMOD(VARKIND::Let, false, VAROPT::Optional),
            MAKE(BindingPattern, "x"),
            IDENT("T")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, VarDefTest4) {
    auto [ast, errors] = parse("ref x = y");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        VARDEF(
            VARMOD(VARKIND::Ref, false, VAROPT::None),
            MAKE(BindingPattern, "x"),
            nullptr,
            IDENT("y")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}
