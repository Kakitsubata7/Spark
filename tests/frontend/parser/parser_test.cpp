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
#define MATCH(scrutinee, ...) MAKE(MatchExpr, scrutinee, std::vector<MatchCase*>{__VA_ARGS__})
#define MATCH_CASE(...) MAKE(MatchCase, __VA_ARGS__)
#define THROW(expr) MAKE(ThrowExpr, expr)

#define BINARY(op, lhs, rhs) MAKE(BinaryExpr, op, lhs, rhs)
#define BINARY_OP BinaryExpr::OpKind

#define PREFIX(op, expr) MAKE(PrefixExpr, op, expr)
#define PREFIX_OP PrefixExpr::OpKind

#define POSTFIX(op, expr) MAKE(PostfixExpr, op, expr)
#define POSTFIX_OP PostfixExpr::OpKind

#define MEMBER(base, member) MAKE(MemberAccessExpr, base, member)

#define CALL(callee, ...) MAKE(CallExpr, callee, std::vector<CallArg*>{__VA_ARGS__})
#define CALL_ARG(...) MAKE(CallArg, __VA_ARGS__)

#define SUBSCRIPT(base, ...) MAKE(SubscriptExpr, base, std::vector<Expr*>{__VA_ARGS__})

#define IDENT(name) MAKE(IdentifierExpr, name)

#define INT(v) MAKE(IntLiteralExpr, v)

#define IDENT_NAME(name) MAKE(IdentifierName, name)

using namespace Spark;
using namespace Spark::FrontEnd;

std::pair<AST, std::vector<Error>> parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    return Parser::parse(iss);
}

TEST(ParserTest, BlockTest1) {
    auto [ast, errors] = parse("{ }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(BLOCK())
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, BlockTest2) {
    auto [ast, errors] = parse("{ a; b; c }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            BLOCK(
                EXPR_STMT(IDENT("a")),
                EXPR_STMT(IDENT("b")),
                EXPR_STMT(IDENT("c"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, IfThenTest) {
    auto [ast, errors] = parse("if foo() then a else b");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            MAKE(IfThenExpr,
                CALL(IDENT("foo")), IDENT("a"), IDENT("b")
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, TryElseTest) {
    auto [ast, errors] = parse("x = try { foo(); throw Error() } else 1");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        ASSIGN(ASSIGN_OP::Assign,
            IDENT("x"),
            MAKE(TryElseExpr,
                BLOCK(
                    EXPR_STMT(CALL(IDENT("foo"))),
                    EXPR_STMT(THROW(CALL(IDENT("Error"))))
                ),
                INT(BigInt(1))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, MatchTest1) {
    auto [ast, errors] = parse("match x { case y => z }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            MATCH(
                IDENT("x"),
                MATCH_CASE(
                    MAKE(BindingPattern, "y"),
                    /* guard */ nullptr,
                    IDENT("z")
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, MatchTest2) {
    auto [ast, errors] = parse("match x { case if cond => y }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            MATCH(
                IDENT("x"),
                MATCH_CASE(
                    /* pattern */ nullptr,
                    IDENT("cond"),
                    IDENT("y")
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, MatchTest3) {
    auto [ast, errors] = parse("match x { case y if cond => z }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            MATCH(
                IDENT("x"),
                MATCH_CASE(
                    MAKE(BindingPattern, "y"),
                    IDENT("cond"),
                    IDENT("z")
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, Match_MultipleCases) {
    auto [ast, errors] = parse(R"(
match x {
    case a => b
    case c if d => e
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            MATCH(
                IDENT("x"),
                MATCH_CASE(MAKE(BindingPattern, "a"), nullptr, IDENT("b")),
                MATCH_CASE(MAKE(BindingPattern, "c"), IDENT("d"), IDENT("e"))
            )
        )
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

TEST(ParserTest, OperatorTest12) {
    auto [ast, errors] = parse("-~!a");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            PREFIX(PREFIX_OP::Neg,
                PREFIX(PREFIX_OP::BitNot,
                    PREFIX(PREFIX_OP::LogNot, IDENT("a"))
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest13) {
    // Make sure postfix binds tighter than prefix
    auto [ast, errors] = parse("-a!");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            PREFIX(PREFIX_OP::Neg,
               POSTFIX(POSTFIX_OP::ForceUnwrap, IDENT("a"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest14) {
    // Make sure postfix binds tighter than prefix
    auto [ast, errors] = parse("!!!a!!!");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            PREFIX(PREFIX_OP::LogNot,
                PREFIX(PREFIX_OP::LogNot,
                    PREFIX(PREFIX_OP::LogNot,
                        POSTFIX(POSTFIX_OP::ForceUnwrap,
                            POSTFIX(POSTFIX_OP::ForceUnwrap,
                                POSTFIX(POSTFIX_OP::ForceUnwrap,
                                    IDENT("a"))))))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest15) {
    auto [ast, errors] = parse("a.b(c)[d]");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        EXPR_STMT(
            SUBSCRIPT(
                CALL(
                    MEMBER(IDENT("a"), IDENT_NAME("b")),
                    CALL_ARG(IDENT("c"))
                ),
                IDENT("d")
            )
        )
    );

    EXPECT_EQ(*ast.root, *root);
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

TEST(ParserTest, FnDefTest1) {
    auto [ast, errors] = parse("fn foo() { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            /* isImmutable */ false,
            IDENT_NAME("foo"),
            /* generics */ std::vector<Expr*>(),
            /* params */ std::vector<FnParam*>(),
            /* captureClause */ nullptr,
            /* returns */ std::vector<FnReturn*>(),
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, FnDefTest2) {
    auto [ast, errors] = parse("fn^ foo[T]() { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            /* isImmutable */ true,
            IDENT_NAME("foo"),
            /* generics */ std::vector<Expr*>{ IDENT("T") },
            /* params */ std::vector<FnParam*>(),
            /* captureClause */ nullptr,
            /* returns */ std::vector<FnReturn*>(),
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, FnDefTest3) {
    auto [ast, errors] = parse("fn foo(x, y) { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            /* isImmutable */ false,
            IDENT_NAME("foo"),
            /* generics */ std::vector<Expr*>(),
            /* params */ std::vector<FnParam*>{
                MAKE(FnParam, nullptr, MAKE(BindingPattern, "x"), nullptr, nullptr),
                MAKE(FnParam, nullptr, MAKE(BindingPattern, "y"), nullptr, nullptr)
            },
            /* captureClause */ nullptr,
            /* returns */ std::vector<FnReturn*>(),
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BLOCK()
        )
    );

    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, FnDefTest4) {
    auto [ast, errors] = parse("fn foo()[] { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            /* isImmutable */ false,
            IDENT_NAME("foo"),
            /* generics */ std::vector<Expr*>(),
            /* params */ std::vector<FnParam*>(),
            MAKE(FnCaptureClause,
                std::vector<FnCapture*>(), false, nullptr
            ),
            /* returns */ std::vector<FnReturn*>(),
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, FnDefTest5) {
    auto [ast, errors] = parse("fn foo() -> Int { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            /* isImmutable */ false,
            IDENT_NAME("foo"),
            /* generics */ std::vector<Expr*>(),
            /* params */ std::vector<FnParam*>(),
            /* captureClause */ nullptr,
            std::vector<FnReturn*>{
                MAKE(FnReturn, FnReturn::RetKind::ByValue, IDENT("Int"))
            },
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, FnDefTest6) {
    // fn foo(x) => x

    auto [ast, errors] = parse("fn foo(x) => x");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            /* isImmutable */ false,
            IDENT_NAME("foo"),
            /* generics */ std::vector<Expr*>(),
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, MAKE(BindingPattern, "x"), nullptr, nullptr)
            },
            /* captureClause */ nullptr,
            /* returns */ std::vector<FnReturn*>(),
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            IDENT("x")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, FnDefTest7) {
    auto [ast, errors] = parse("fn foo() throw Error { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            /* isImmutable */ false,
            IDENT_NAME("foo"),
            /* generics */ std::vector<Expr*>(),
            /* params */ std::vector<FnParam*>(),
            /* captureClause */ nullptr,
            /* returns */ std::vector<FnReturn*>(),
            /* isThrowing */ true,
            /* throwExpr */ IDENT("Error"),
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, TypeDefTest1) {
    auto [ast, errors] = parse("struct Foo { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(TypeDefStmt,
            TypeDefStmt::TypeKind::Struct,
            /* isImmutable */ false,
            IDENT_NAME("Foo"),
            /* template */ std::vector<Expr*>(),
            /* bases */ std::vector<Expr*>(),
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, TypeDefTest2) {
    auto [ast, errors] = parse("class^ Foo { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(TypeDefStmt,
            TypeDefStmt::TypeKind::Class,
            /* isImmutable */ true,
            IDENT_NAME("Foo"),
            /* template */ std::vector<Expr*>(),
            /* bases */ std::vector<Expr*>(),
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, TypeDefTest3) {
    auto [ast, errors] = parse("struct Foo : Bar, Baz { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(TypeDefStmt,
            TypeDefStmt::TypeKind::Struct,
            /* isImmutable */ false,
            IDENT_NAME("Foo"),
            /* template */ std::vector<Expr*>(),
            /* bases */ std::vector<Expr*>{
                IDENT("Bar"),
                IDENT("Baz")
            },
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, TypeDefTest4) {
    auto [ast, errors] = parse("enum class Color { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(TypeDefStmt,
            TypeDefStmt::TypeKind::EnumClass,
            /* isImmutable */ false,
            IDENT_NAME("Color"),
            /* template */ std::vector<Expr*>(),
            /* bases */ std::vector<Expr*>(),
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, TypeDefTest5) {
    auto [ast, errors] = parse("struct Foo[T, U] { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(TypeDefStmt,
            TypeDefStmt::TypeKind::Struct,
            /* isImmutable */false,
            IDENT_NAME("Foo"),
            std::vector<Expr*>{
                IDENT("T"),
                IDENT("U")
            },
            /* bases */ std::vector<Expr*>(),
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, IfStmtTest1) {
    auto [ast, errors] = parse("if a { b }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(IfStmt,
            IDENT("a"),
            BLOCK(
                EXPR_STMT(IDENT("b"))
            ),
            /* elseBody */ nullptr
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, IfStmtTest2) {
    auto [ast, errors] = parse("if a { b } else { c }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(IfStmt,
            IDENT("a"),
            BLOCK(
                EXPR_STMT(IDENT("b"))
            ),
            BLOCK(
                EXPR_STMT(IDENT("c"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, IfStmtTest3) {
    auto [ast, errors] = parse("if a { b } else if c { d }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(IfStmt,
            IDENT("a"),
            BLOCK(
                EXPR_STMT(IDENT("b"))
            ),
            BLOCK(
               MAKE(IfStmt,
                   IDENT("c"),
                   BLOCK(
                       EXPR_STMT(IDENT("d"))
                   ),
                   /* elseBody */ nullptr
               )
           )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, IfStmtTest4) {
    auto [ast, errors] = parse("if a { b } else if c { d } else { e }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(IfStmt,
            IDENT("a"),
            BLOCK(
                EXPR_STMT(IDENT("b"))
            ),
            BLOCK(
                MAKE(IfStmt,
                    IDENT("c"),
                    BLOCK(
                        EXPR_STMT(IDENT("d"))
                    ),
                    BLOCK(
                        EXPR_STMT(IDENT("e"))
                    )
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

