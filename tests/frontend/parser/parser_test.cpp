#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "frontend/ast.hpp"
#include "frontend/parser.hpp"
#include "utils/error.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

#define MAKE(type, ...) ast.make<type>(Location(0, 0), Location(0, 0), __VA_ARGS__)

#define VARMOD(kind, isImmut, opt) MAKE(VarModifier, kind, isImmut, opt)
#define VARKIND VarModifier::VarKind
#define VAROPT VarModifier::Optionality
#define VARDEF(mod, pattern, ...) MAKE(VarDefStmt, mod, pattern, __VA_ARGS__)

#define ASSIGN(op, lhs, rhs) MAKE(AssignStmt, op, lhs, rhs)
#define ASSIGN_OP AssignStmt::OpKind

#define BREAK ast.make<BreakStmt>(Location(0, 0), Location(0, 0))
#define CONTINUE ast.make<ContinueStmt>(Location(0, 0), Location(0, 0))
#define RETURN(...) MAKE(ReturnStmt, __VA_ARGS__)

#define BLOCK(...) MAKE(BlockExpr, std::vector<Node*>{__VA_ARGS__})
#define IFTHEN(cond, t, f) MAKE(IfThenExpr, cond, t, f)
#define TRYELSE(t, e) MAKE(TryElseExpr, t, e)
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

#define INT(v) MAKE(LiteralExpr, IntLiteral(BigInt(v)))
#define REAL(v) MAKE(LiteralExpr, RealLiteral(BigReal(v))
#define BOOL(v) MAKE(LiteralExpr, BoolLiteral(v))
#define STRING(s) MAKE(LiteralExpr, StringLiteral(s))
#define NIL MAKE(LiteralExpr, NilLiteral())
#define VOID MAKE(LiteralExpr, VoidLiteral())

#define IDENT(s) MAKE(NameExpr, IdentifierName(s))

#define COLLECTION(...) MAKE(CollectionExpr, std::vector<Expr*>{__VA_ARGS__})

#define BIND_PAT(name) MAKE(BindingPattern, NAME(name))
#define TUP_PAT(...) MAKE(TuplePattern, std::vector<Pattern*>{__VA_ARGS__})

Name NAME(const char* s) { return Name(IdentifierName(s)); }
Name NAME(Name name) { return Name(std::move(name)); }
#define DISCARD DiscardName()
#define CONSTRUCTOR ConstructorName()
#define DESTRUCTOR DestructorName()
#define SELF SelfName()

#define PATH(...) MAKE(Path, std::vector<PathSeg*>{__VA_ARGS__})
#define PATH_SEG(name, ...) MAKE(PathSeg, name, std::vector<Expr*>{__VA_ARGS__})

std::pair<AST, std::vector<Error>> parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    return Parser::parse(iss);
}

TEST(ParserTest, LambdaTest1) {
    auto [ast, errors] = parse("fn (x, y) => x + y");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(LambdaExpr,
            /* isImmutable */ false,
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("x")), nullptr, nullptr),
                MAKE(FnParam, nullptr, BIND_PAT(NAME("y")), nullptr, nullptr)
            },
            /* captureClause */ nullptr,
            /* returns */ std::vector<FnReturn*>(),
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BINARY(BINARY_OP::Add, IDENT("x"), IDENT("y"))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, LambdaTest2) {
    auto [ast, errors] = parse("fn^ (x: Int, y: Int) -> Int => x * y");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(LambdaExpr,
            /* isImmutable */ true,
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("x")), IDENT("Int"), nullptr),
                MAKE(FnParam, nullptr, BIND_PAT(NAME("y")), IDENT("Int"), nullptr)
            },
            /* captureClause */ nullptr,
            std::vector<FnReturn*>{
                MAKE(FnReturn, FnReturn::RetKind::ByValue, IDENT("Int"))
            },
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BINARY(BINARY_OP::Mul, IDENT("x"), IDENT("y"))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, LambdaTest3) {
    auto [ast, errors] = parse("fn (x)[ref y] => x + y");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(LambdaExpr,
            /* isImmutable */ false,
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("x")), nullptr, nullptr)
            },
            MAKE(FnCaptureClause,
                std::vector<FnCapture*>{
                    MAKE(FnCapture,
                        VARMOD(VARKIND::Ref, false, VAROPT::None),
                        BIND_PAT(NAME("y")))
                },
                false,
                nullptr
            ),
            /* returns */ std::vector<FnReturn*>(),
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BINARY(BINARY_OP::Add, IDENT("x"), IDENT("y"))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, LambdaTest4) {
    auto [ast, errors] = parse("fn (x) throw Error => risky(x)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(LambdaExpr,
            /* isImmutable */ false,
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("x")), nullptr, nullptr)
            },
            /* captureClause */ nullptr,
            /* returns */ std::vector<FnReturn*>(),
            /* isThrowing */ true,
            /* throwExpr */ IDENT("Error"),
            CALL(IDENT("risky"), CALL_ARG(IDENT("x")))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, LambdaTest5) {
    auto [ast, errors] = parse("fn (x: Int) -> Bool => x % 2 == 0");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(LambdaExpr,
            /* isImmutable */ false,
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("x")), IDENT("Int"), nullptr)
            },
            /* captureClause */ nullptr,
            std::vector<FnReturn*>{
                MAKE(FnReturn, FnReturn::RetKind::ByValue, IDENT("Bool"))
            },
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BINARY(
                BINARY_OP::Eq,
                BINARY(BINARY_OP::Mod, IDENT("x"), INT(2)),
                INT(0)
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, BlockTest1) {
    auto [ast, errors] = parse("{ }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BLOCK()
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, BlockTest2) {
    auto [ast, errors] = parse("{ a; b; c }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BLOCK(
            IDENT("a"),
            IDENT("b"),
            IDENT("c")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, DISABLED_BlockTest3) {
    auto [ast, errors] = parse(R"(
{
    a
    b
    c
})");
    EXPECT_TRUE(errors.empty()) << errors.size();

    Node* root = BLOCK(
        BLOCK(
            IDENT("a"),
            IDENT("b"),
            IDENT("c")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, IfThenTest1) {
    auto [ast, errors] = parse("if foo() then a else b");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        IFTHEN(
            CALL(IDENT("foo")), IDENT("a"), IDENT("b")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, TryElseTest1) {
    auto [ast, errors] = parse("x = try { foo(); throw Error() } else 1");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        ASSIGN(ASSIGN_OP::Assign,
            IDENT("x"),
            TRYELSE(
                BLOCK(
                    CALL(IDENT("foo")),
                    THROW(CALL(IDENT("Error")))
                ),
                INT(1)
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, MatchTest1) {
    auto [ast, errors] = parse("match x { case y => z }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MATCH(
            IDENT("x"),
            MATCH_CASE(
                MAKE(BindingPattern, NAME("y")),
                /* guard */ nullptr,
                IDENT("z")
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, MatchTest2) {
    auto [ast, errors] = parse("match x { case if cond => y }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MATCH(
            IDENT("x"),
            MATCH_CASE(
                /* pattern */ nullptr,
                IDENT("cond"),
                IDENT("y")
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, MatchTest3) {
    auto [ast, errors] = parse("match x { case y if cond => z }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MATCH(
            IDENT("x"),
            MATCH_CASE(
                MAKE(BindingPattern, NAME("y")),
                IDENT("cond"),
                IDENT("z")
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
        MATCH(
            IDENT("x"),
            MATCH_CASE(MAKE(BindingPattern, NAME("a")), nullptr, IDENT("b")),
            MATCH_CASE(MAKE(BindingPattern, NAME("c")), IDENT("d"), IDENT("e"))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest1) {
    // Making sure `*` binds tighter than `+`
    auto [ast, errors] = parse("a + b * c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BINARY(BINARY_OP::Add,
            IDENT("a"), BINARY(BINARY_OP::Mul, IDENT("b"), IDENT("c"))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest2) {
    // Making sure `*` binds tighter than `+`
    auto [ast, errors] = parse("a * b + c * d");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BINARY(BINARY_OP::Add,
            BINARY(BINARY_OP::Mul, IDENT("a"), IDENT("b")),
            BINARY(BINARY_OP::Mul, IDENT("c"), IDENT("d")))
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest3) {
    // Making sure arithmetic of the same operator is left-associative
    auto [ast, errors] = parse("a - b - c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BINARY(BINARY_OP::Sub,
            BINARY(BINARY_OP::Sub, IDENT("a"), IDENT("b")),
            IDENT("c")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest4) {
    // Making sure prefix binds tighter than binary
    auto [ast, errors] = parse("-a * b");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BINARY(BINARY_OP::Mul,
            PREFIX(PREFIX_OP::Neg, IDENT("a")),
            IDENT("b")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest5) {
    // Making sure prefix binds tighter than binary
    auto [ast, errors] = parse("-a + b");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BINARY(BINARY_OP::Add,
           PREFIX(PREFIX_OP::Neg, IDENT("a")),
           IDENT("b")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest6) {
    // Making sure expression in parentheses binds tighter than others
    auto [ast, errors] = parse("(a + b) * c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BINARY(BINARY_OP::Mul,
           BINARY(BINARY_OP::Add, IDENT("a"), IDENT("b")),
           IDENT("c")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest7) {
    // Making sure comparison binds looser than arithmetic
    auto [ast, errors] = parse("a + b < c * d");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BINARY(BINARY_OP::Lt,
            BINARY(BINARY_OP::Add, IDENT("a"), IDENT("b")),
            BINARY(BINARY_OP::Mul, IDENT("c"), IDENT("d"))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest8) {
    // Making sure `&&` binds tighter than `||`
    auto [ast, errors] = parse("a || b && c");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        BINARY(BINARY_OP::LogOr,
            IDENT("a"),
            BINARY(BINARY_OP::LogAnd, IDENT("b"), IDENT("c"))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest9) {
    // Binary operator stress test
    auto [ast, errors] = parse("a + b * c < d && e || f");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
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
        PREFIX(PREFIX_OP::Neg,
            PREFIX(PREFIX_OP::BitNot,
                PREFIX(PREFIX_OP::LogNot, IDENT("a"))
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
        PREFIX(PREFIX_OP::Neg,
           POSTFIX(POSTFIX_OP::ForceUnwrap, IDENT("a"))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest14) {
    // Make sure postfix binds tighter than prefix
    auto [ast, errors] = parse("!!!a!!!");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        PREFIX(PREFIX_OP::LogNot,
            PREFIX(PREFIX_OP::LogNot,
                PREFIX(PREFIX_OP::LogNot,
                    POSTFIX(POSTFIX_OP::ForceUnwrap,
                        POSTFIX(POSTFIX_OP::ForceUnwrap,
                            POSTFIX(POSTFIX_OP::ForceUnwrap,
                                IDENT("a"))))))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, OperatorTest15) {
    auto [ast, errors] = parse("a.b(c)[d]");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        SUBSCRIPT(
            CALL(
                MEMBER(IDENT("a"), NAME("b")),
                CALL_ARG(IDENT("c"))
            ),
            IDENT("d")
        )
    );

    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, TypeofTest) {
    auto [ast, errors] = parse("typeof(x + y)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(TypeofExpr,
             BINARY(BINARY_OP::Add, IDENT("x"), IDENT("y"))
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
            BIND_PAT(NAME("x")),
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
            MAKE(BindingPattern, NAME("x")),
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
            MAKE(BindingPattern, NAME("x")),
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
            MAKE(BindingPattern, NAME("x")),
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
            NAME("foo"),
            /* generics */ std::vector<Name>(),
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
            NAME("foo"),
            /* generics */ std::vector<Name>{ NAME("T") },
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
            NAME("foo"),
            /* generics */ std::vector<Name>(),
            /* params */ std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("x")), nullptr, nullptr),
                MAKE(FnParam, nullptr, BIND_PAT(NAME("y")), nullptr, nullptr)
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
            NAME("foo"),
            /* generics */ std::vector<Name>(),
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
            NAME("foo"),
            /* generics */ std::vector<Name>(),
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
            NAME("foo"),
            /* generics */ std::vector<Name>(),
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("x")), nullptr, nullptr)
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
            NAME("foo"),
            /* generics */ std::vector<Name>(),
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
            NAME("Foo"),
            /* template */ std::vector<Name>(),
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
            NAME("Foo"),
            /* template */ std::vector<Name>(),
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
            NAME("Foo"),
            /* template */ std::vector<Name>(),
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
            NAME("Color"),
            /* generics */ std::vector<Name>(),
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
            /* isImmutable */ false,
            NAME("Foo"),
            std::vector<Name>{
                NAME("T"),
                NAME("U")
            },
            /* bases */ std::vector<Expr*>(),
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, CaseDefTest1) {
    auto [ast, errors] = parse(R"(
enum Color {
    case Red;
    case Yellow;
    case Blue;
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(TypeDefStmt,
            TypeDefStmt::TypeKind::Enum,
            /* isImmutable */ false,
            NAME("Color"),
            /* generics */ std::vector<Name>(),
            /* bases */ std::vector<Expr*>(),
            BLOCK(
                MAKE(CaseDefStmt, NAME("Red")),
                MAKE(CaseDefStmt, NAME("Yellow")),
                MAKE(CaseDefStmt, NAME("Blue")),
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, CaseDefTest2) {
    auto [ast, errors] = parse(R"(
enum OpKind : UInt32 {
    case None = 0;
    case Add  = 1;
    case Sub  = 2;
    case Mul  = 3;
    case Div  = 4;
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(TypeDefStmt,
            TypeDefStmt::TypeKind::Enum,
            /* isImmutable */ false,
            NAME("OpKind"),
            /* generics */ std::vector<Name>(),
            /* bases */ std::vector<Expr*>{
                IDENT("UInt32")
            },
            BLOCK(
                MAKE(CaseDefStmt, NAME("None"), INT(0)),
                MAKE(CaseDefStmt, NAME("Add"),  INT(1)),
                MAKE(CaseDefStmt, NAME("Sub"),  INT(2)),
                MAKE(CaseDefStmt, NAME("Mul"),  INT(3)),
                MAKE(CaseDefStmt, NAME("Div"),  INT(4)),
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

TEST(ParserTest, IfStmtTest1) {
    auto [ast, errors] = parse("if a { b }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(IfStmt,
            IDENT("a"),
            BLOCK(
                IDENT("b")
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
                IDENT("b")
            ),
            BLOCK(
                IDENT("c")
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
                IDENT("b")
            ),
            BLOCK(
               MAKE(IfStmt,
                   IDENT("c"),
                   BLOCK(
                       IDENT("d")
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
                IDENT("b")
            ),
            BLOCK(
                MAKE(IfStmt,
                    IDENT("c"),
                    BLOCK(
                        IDENT("d")
                    ),
                    BLOCK(
                        IDENT("e")
                    )
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, WhileStmtTest1) {
    auto [ast, errors] = parse("while a { b }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(WhileStmt,
            IDENT("a"),
            BLOCK(
                IDENT("b")
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, DoWhileStmtTest1) {
    auto [ast, errors] = parse("do { a } while b");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(DoWhileStmt,
            BLOCK(
                IDENT("a")
            ),
            IDENT("b")
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ForStmtTest1) {
    auto [ast, errors] = parse("for i in 0...10 { print(i) }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ForStmt,
            BIND_PAT(NAME("i")),
            BINARY(BINARY_OP::Range, INT(0), INT(10)),
            BLOCK(
                CALL(IDENT("print"), CALL_ARG(IDENT("i")))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ForStmtTest2) {
    auto [ast, errors] = parse("for (x, y) in tuples { foo(x, y) }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ForStmt,
            TUP_PAT(BIND_PAT(NAME("x")), BIND_PAT(NAME("y"))),
            IDENT("tuples"),
            BLOCK(
                CALL(IDENT("foo"), CALL_ARG(IDENT("x")), CALL_ARG(IDENT("y")))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ForStmtTest3) {
    auto [ast, errors] = parse("for i in [1, 2, 3] { y += x }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ForStmt,
            BIND_PAT(NAME("i")),
            COLLECTION(INT(1), INT(2), INT(3)),
            BLOCK(
                ASSIGN(ASSIGN_OP::AddAssign, IDENT("y"), IDENT("x"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ForStmtTest4) {
    auto [ast, errors] = parse("for j in -10..<10 { print('value: ' + j) }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ForStmt,
            BIND_PAT(NAME("j")),
            BINARY(BINARY_OP::RangeExcl, PREFIX(PREFIX_OP::Neg, INT(10)), INT(10)),
            BLOCK(
                CALL(IDENT("print"), CALL_ARG(BINARY(BINARY_OP::Add, STRING("value: "), IDENT("j"))))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ForStmtTest5) {
    auto [ast, errors] = parse(R"(
fn findFirstEven(nums: Array[Int]) -> Int? {
    for x in nums {
        if x < 0 {
            continue;
        };
        if x % 2 == 0 {
            return x;
        };
        if x > 1000000 {
            break;
        };
    };
    return nil;
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            /* isImmutable */ false,
            NAME("findFirstEven"),
            /* generics */ std::vector<Name>(),
            /* params */ std::vector<FnParam*>{
                MAKE(FnParam,
                    /* mod */ nullptr,
                    BIND_PAT(NAME("nums")),
                    SUBSCRIPT(IDENT("Array"), IDENT("Int")),
                    /* def */ nullptr
                )
            },
            /* captureClause */ nullptr,
            /* returns */ std::vector<FnReturn*>{
                MAKE(FnReturn,
                    FnReturn::RetKind::ByValue, POSTFIX(POSTFIX_OP::Optional, IDENT("Int"))
                )
            },
            /* isThrowing */ false,
            /* throwExpr */ nullptr,
            BLOCK(
                MAKE(ForStmt,
                    BIND_PAT(NAME("x")),
                    IDENT("nums"),
                    BLOCK(
                        MAKE(IfStmt,
                            BINARY(BINARY_OP::Lt, IDENT("x"), INT(0)),
                            BLOCK(
                                CONTINUE
                            ),
                            /* elseBody */ nullptr
                        ),

                        MAKE(IfStmt,
                            BINARY(BINARY_OP::Eq,
                                BINARY(BINARY_OP::Mod, IDENT("x"), INT(2)), INT(0)),
                            BLOCK(
                                RETURN(IDENT("x"))
                            ),
                            /* elseBody */ nullptr
                        ),

                        MAKE(IfStmt,
                            BINARY(BINARY_OP::Gt, IDENT("x"), INT(1000000)),
                            BLOCK(
                                BREAK
                            ),
                            /* elseBody */ nullptr
                        )
                    )
                ),

                RETURN(NIL)
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ModuleStmtTest1) {
    auto [ast, errors] = parse("module Foo { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ModuleStmt,
            PATH(PATH_SEG(NAME("Foo"))),
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ModuleStmtTest2) {
    auto [ast, errors] = parse("module Foo.Bar { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ModuleStmt,
            PATH(PATH_SEG(NAME("Foo")), PATH_SEG(NAME("Bar"))),
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ModuleStmtTest3) {
    auto [ast, errors] = parse("module Foo.Bar.Baz { }");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ModuleStmt,
            PATH(PATH_SEG(NAME("Foo")), PATH_SEG(NAME("Bar")), PATH_SEG(NAME("Baz"))),
            BLOCK()
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ExportStmtTest1) {
    auto [ast, errors] = parse(
R"(
export module Foo {
    export let x: Bar = Bar()
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ExportStmt,
            MAKE(ModuleStmt,
                PATH(PATH_SEG(NAME("Foo"))),
                BLOCK(
                    MAKE(ExportStmt,
                        VARDEF(
                            VARMOD(VARKIND::Let, false, VAROPT::None),
                            BIND_PAT(NAME("x")),
                            IDENT("Bar"),
                            CALL(IDENT("Bar"))
                        )
                    )
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ExportStmtTest2) {
    auto [ast, errors] = parse(
R"(
export fn^ max[T](a: T, b: T) -> T throw Error {
    return myMax(a, b)
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ExportStmt,
            MAKE(FnDefStmt,
                /* isImmutable */ true,
                NAME("max"),
                /* generics */ std::vector<Name>{
                    NAME("T")
                },
                /* params */ std::vector<FnParam*>{
                    MAKE(FnParam, nullptr, BIND_PAT(NAME("a")), IDENT("T"), nullptr),
                    MAKE(FnParam, nullptr, BIND_PAT(NAME("b")), IDENT("T"), nullptr)
                },
                /* captureClause */ nullptr,
                /* returns */ std::vector<FnReturn*>{
                    MAKE(FnReturn, FnReturn::RetKind::ByValue, IDENT("T"))
                },
                /* isThrowing */ true,
                /* throwExpr */ IDENT("Error"),
                BLOCK(
                    RETURN(CALL(IDENT("myMax"), CALL_ARG(IDENT("a")), CALL_ARG(IDENT("b"))))
                )
            )
        )
    );

    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ExportStmtTest3) {
    auto [ast, errors] = parse(
R"(
export class Box[T] : Container { }
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ExportStmt,
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::Class,
                /* isImmutable */ false,
                NAME("Box"),
                /* generics */ std::vector<Name>{
                    NAME("T")
                },
                /* bases */ std::vector<Expr*>{
                    IDENT("Container")
                },
                BLOCK()
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ExportTypeDefTest2) {
    auto [ast, errors] = parse(
R"(
export class Box[T] : Container {
    let value: T;

    fn get() -> T {
        return value;
    }
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ExportStmt,
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::Class,
                /* isImmutable */ false,
                NAME("Box"),
                /* generics */ std::vector<Name>{
                    NAME("T")
                },
                /* bases */ std::vector<Expr*>{
                    IDENT("Container")
                },
                BLOCK(
                    VARDEF(
                        VARMOD(VARKIND::Let, false, VAROPT::None),
                        BIND_PAT(NAME("value")),
                        IDENT("T"),
                        nullptr
                    ),
                    MAKE(FnDefStmt,
                        /* isImmutable */ false,
                        NAME("get"),
                        /* generics */ std::vector<Name>(),
                        /* params */ std::vector<FnParam*>(),
                        /* captureClause */ nullptr,
                        /* returns */ std::vector<FnReturn*>{
                            MAKE(FnReturn, FnReturn::RetKind::ByValue, IDENT("T"))
                        },
                        /* isThrowing */ false,
                        /* throwExpr */ nullptr,
                        BLOCK(
                            RETURN(IDENT("value"))
                        )
                    )
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ImportStmtTest1) {
    auto [ast, errors] = parse("from Foo import Bar");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ImportStmt,
            PATH(PATH_SEG(NAME("Foo"))),
            std::vector<ImportItem*>{
                MAKE(ImportItem,
                    PATH(PATH_SEG(NAME("Bar"))),
                    std::nullopt
                )
            }
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ImportStmtTest2) {
    auto [ast, errors] = parse("from Foo import Bar, Baz as Qux");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ImportStmt,
            PATH(PATH_SEG(NAME("Foo"))),
            std::vector<ImportItem*>{
                MAKE(ImportItem,
                    PATH(PATH_SEG(NAME("Bar"))),
                    std::nullopt
                ),
                MAKE(ImportItem,
                    PATH(PATH_SEG(NAME("Baz"))),
                    NAME("Qux")
                )
            }
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ImportStmtTest3) {
    auto [ast, errors] = parse(
        "from Foo.Bar[Int] import Baz[Bool] as BazBool, Qux"
    );
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ImportStmt,
            PATH(
                PATH_SEG(NAME("Foo")),
                PATH_SEG(NAME("Bar"), IDENT("Int"))
            ),
            std::vector<ImportItem*>{
                MAKE(ImportItem,
                    PATH(
                        PATH_SEG(NAME("Baz"), IDENT("Bool"))
                    ),
                    NAME("BazBool")
                ),
                MAKE(ImportItem,
                    PATH(PATH_SEG(NAME("Qux"))),
                    std::nullopt
                )
            }
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ImportAllStmtTest1) {
    auto [ast, errors] = parse("from Foo import *");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ImportAllStmt,
            PATH(PATH_SEG(NAME("Foo")))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ImportAllStmtTest2) {
    auto [ast, errors] = parse("from Foo.Bar[Int] import *");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ImportAllStmt,
            PATH(PATH_SEG(NAME("Foo")), PATH_SEG(NAME("Bar"), IDENT("Int")))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, ImportAllStmtTest3) {
    auto [ast, errors] = parse("from Foo.Bar[Int].Baz[Bool] import *");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(ImportAllStmt,
            PATH(
                PATH_SEG(NAME("Foo")),
                PATH_SEG(NAME("Bar"), IDENT("Int")),
                PATH_SEG(NAME("Baz"), IDENT("Bool"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, UndefineStmtTest1) {
    auto [ast, errors] = parse("undefine Foo");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(UndefineStmt,
            PATH(PATH_SEG(NAME("Foo")))
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, UndefineStmtTest2) {
    auto [ast, errors] = parse("undefine Foo.Bar[Int]");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(UndefineStmt,
            PATH(
                PATH_SEG(NAME("Foo")),
                PATH_SEG(NAME("Bar"), IDENT("Int"))
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, UndefineStmtTest3) {
    auto [ast, errors] = parse("undefine Foo.Bar[Int].constructor");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(UndefineStmt,
            PATH(
                PATH_SEG(NAME("Foo")),
                PATH_SEG(NAME("Bar"), IDENT("Int")),
                PATH_SEG(CONSTRUCTOR)
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, Test1) {
    auto [ast, errors] = parse(
R"(
fn fact(n: Int) -> Int {
    if n <= 1 {
        return 1;
    };

    return n * fact(n - 1);
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            false,
            NAME("fact"),
            std::vector<Name>(),
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("n")), IDENT("Int"), nullptr)
            },
            nullptr,
            std::vector<FnReturn*>{
                MAKE(FnReturn, FnReturn::RetKind::ByValue, IDENT("Int"))
            },
            false,
            nullptr,
            BLOCK(
                MAKE(IfStmt,
                    BINARY(BINARY_OP::Le, IDENT("n"), INT(1)),
                    BLOCK(
                        RETURN(INT(1))
                    ),
                    nullptr
                ),
                RETURN(
                    BINARY(
                        BINARY_OP::Mul,
                        IDENT("n"),
                        CALL(
                            IDENT("fact"),
                            CALL_ARG(BINARY(BINARY_OP::Sub, IDENT("n"), INT(1)))
                        )
                    )
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, Test2) {
    auto [ast, errors] = parse(
R"(
fn fib(n: Int) -> Int {
    if n <= 1 {
        return n
    };

    return fib(n - 1) + fib(n - 2)
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            false,
            NAME("fib"),
            std::vector<Name>(),
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("n")), IDENT("Int"), nullptr)
            },
            nullptr,
            std::vector<FnReturn*>{
                MAKE(FnReturn, FnReturn::RetKind::ByValue, IDENT("Int"))
            },
            false,
            nullptr,
            BLOCK(
                MAKE(IfStmt,
                    BINARY(BINARY_OP::Le, IDENT("n"), INT(1)),
                    BLOCK(
                        RETURN(IDENT("n"))
                    ),
                    nullptr
                ),
                RETURN(
                    BINARY(
                        BINARY_OP::Add,
                        CALL(IDENT("fib"),
                            CALL_ARG(BINARY(BINARY_OP::Sub, IDENT("n"), INT(1)))
                        ),
                        CALL(IDENT("fib"),
                            CALL_ARG(BINARY(BINARY_OP::Sub, IDENT("n"), INT(2)))
                        )
                    )
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, Test3) {
    auto [ast, errors] = parse(
R"(
fn firstEven(nums: Array[Int]) -> Int? {
    for x in nums {
        if x < 0 {
            continue;
        };

        if x % 2 == 0 {
            return x;
        };
    };

    return nil;
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            false,
            NAME("firstEven"),
            std::vector<Name>(),
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("nums")),
                     SUBSCRIPT(IDENT("Array"), IDENT("Int")), nullptr)
            },
            nullptr,
            std::vector<FnReturn*>{
                MAKE(FnReturn,
                    FnReturn::RetKind::ByValue,
                    POSTFIX(POSTFIX_OP::Optional, IDENT("Int"))
                )
            },
            false,
            nullptr,
            BLOCK(
                MAKE(ForStmt,
                    BIND_PAT(NAME("x")),
                    IDENT("nums"),
                    BLOCK(
                        MAKE(IfStmt,
                            BINARY(BINARY_OP::Lt, IDENT("x"), INT(0)),
                            BLOCK(CONTINUE),
                            nullptr
                        ),
                        MAKE(IfStmt,
                            BINARY(
                                BINARY_OP::Eq,
                                BINARY(BINARY_OP::Mod, IDENT("x"), INT(2)),
                                INT(0)
                            ),
                            BLOCK(RETURN(IDENT("x"))),
                            nullptr
                        )
                    )
                ),
                RETURN(NIL)
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, Test4) {
    auto [ast, errors] = parse(
R"(
fn makeAdder(base: Int) -> Int -> Int {
    return fn (x) [base] => x + base;
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(FnDefStmt,
            false,
            NAME("makeAdder"),
            std::vector<Name>(),
            std::vector<FnParam*>{
                MAKE(FnParam, nullptr, BIND_PAT(NAME("base")), IDENT("Int"), nullptr)
            },
            nullptr,
            std::vector<FnReturn*>{
                MAKE(FnReturn,
                    FnReturn::RetKind::ByValue,
                    BINARY(
                        BINARY_OP::FuncType,
                        IDENT("Int"),
                        IDENT("Int")
                    )
                )
            },
            false,
            nullptr,
            BLOCK(
                RETURN(
                    MAKE(LambdaExpr,
                        false,
                        std::vector<FnParam*>{
                            MAKE(FnParam, nullptr, BIND_PAT(NAME("x")), nullptr, nullptr)
                        },
                        MAKE(FnCaptureClause,
                            std::vector<FnCapture*>{
                                MAKE(FnCapture, nullptr, BIND_PAT(NAME("base")))
                            },
                            false,
                            nullptr
                        ),
                        std::vector<FnReturn*>(),
                        false,
                        nullptr,
                        BINARY(BINARY_OP::Add, IDENT("x"), IDENT("base"))
                    )
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, Test5) {
    auto [ast, errors] = parse(
R"(
struct Counter {
    let value: Int;

    fn inc() -> Int {
        return value + 1;
    }
}
)");
    EXPECT_TRUE(errors.empty());

    Node* root = BLOCK(
        MAKE(TypeDefStmt,
            TypeDefStmt::TypeKind::Struct,
            false,
            NAME("Counter"),
            std::vector<Name>(),
            std::vector<Expr*>(),
            BLOCK(
                VARDEF(
                    VARMOD(VARKIND::Let, false, VAROPT::None),
                    BIND_PAT(NAME("value")),
                    IDENT("Int"),
                    nullptr
                ),
                MAKE(FnDefStmt,
                    false,
                    NAME("inc"),
                    std::vector<Name>(),
                    std::vector<FnParam*>(),
                    nullptr,
                    std::vector<FnReturn*>{
                        MAKE(FnReturn, FnReturn::RetKind::ByValue, IDENT("Int"))
                    },
                    false,
                    nullptr,
                    BLOCK(
                        RETURN(
                            BINARY(
                                BINARY_OP::Add,
                                IDENT("value"),
                                INT(1)
                            )
                        )
                    )
                )
            )
        )
    );
    EXPECT_EQ(*ast.root, *root);
}

TEST(ParserTest, TestA) {
    auto [ast, errors] = parse(
R"(
module Math {
    export fn fact(n: Int) -> Int {
        if n <= 1 {
            return 1
        };

        return n * fact(n - 1)
    }
};

from Math import fact;

fn main() -> Int {
    return fact(5);
}
)");
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, TestB) {
    auto [ast, errors] = parse(
R"(
struct Box[T] {
    let value: T;

    fn map(f: T -> T) -> T {
        return f(value);
    }
};

fn makeInc(n: Int) -> Int -> Int {
    return fn (x) [n] => x + n;
}
)");
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, TestC) {
    auto [ast, errors] = parse(
R"(
fn find(nums: Array[Int]) -> Int? {
    for x in nums {
        if x < 0 {
            continue;
        };

        if x == 0 {
            break;
        };

        if x % 2 == 0 {
            return x;
        };
    };

    return nil;
}
)");
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, TestD) {
    auto [ast, errors] = parse(
R"(
fn riskyDivide(a: Int, b: Int) -> Int throw Error {
    if b == 0 {
        throw Error();
    };

    return a / b;
};

fn safeCompute(f: (Int, Int) -> Int, x: Int, y: Int) -> Int {
    return f(x, y);
}
)");
    EXPECT_TRUE(errors.empty());
}

TEST(ParserTest, TestE) {
    auto [ast, errors] = parse(
R"(
module Util {
    export fn apply[T](x: T, f: T -> T) -> T {
        return f(x);
    }
};

from Util import apply;

fn main() -> Int {
    let inc = fn (x: Int) => x + 1;
    return apply(41, inc);
}
)");
    EXPECT_TRUE(errors.empty()) << errors[0].message;
}
