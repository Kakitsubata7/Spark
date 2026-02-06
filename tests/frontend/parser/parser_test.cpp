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

#define IDENT(s) MAKE(NameExpr, NAME(s))

#define COLLECTION(...) MAKE(CollectionExpr, std::vector<Expr*>{__VA_ARGS__})

#define BIND_PAT(name) MAKE(BindingPattern, name)
#define TUP_PAT(...) MAKE(TuplePattern, std::vector<Pattern*>{__VA_ARGS__})

#define NAME(s) MAKE(Name, ast.intern(NameValue::identifier(s)))
#define DISCARD MAKE(Name, ast.intern(NameValue::discard()))
#define CONSTRUCTOR MAKE(Name, ast.intern(NameValue::constructor()))
#define DESTRUCTOR MAKE(Name, ast.intern(NameValue::destructor()))
#define SELF MAKE(Name, ast.intern(NameValue::self()))

#define PATH(...) MAKE(Path, std::vector<PathSeg*>{__VA_ARGS__})
#define PATH_SEG(name, ...) MAKE(PathSeg, name, std::vector<Expr*>{__VA_ARGS__})

static std::pair<AST, std::vector<Error>> parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    return Parser::parse(iss);
}

#define PARSE() \
({ \
    std::istringstream iss{std::string(source)};                    \
    ParseResult result = Parser::parse(iss);                        \
    if (result.diagnostics.hasError()) {                            \
        std::ostringstream oss;                                     \
        FAIL() << "error when parsing source: \n" << source << "\n" \
               << oss.str();                                        \
    }                                                               \
    result.ast;                                                     \
})

#define PARSE_EXPECT(source, expected)                            \
{                                                                 \
    AST ast = std::move(PARSE(source));                           \
    EXPECT_TRUE(actualAST.root->equalsStructurally(*(expected))); \
}

#define PARSE_EXPECT_SUCCESS(source) \
{                                    \
    auto [_, errors] = parse(source);    \
    EXPECT_TRUE(errors.empty());         \
}

#define PARSE_EXPECT_ERROR(source)    \
{                                     \
    auto [_, errors] = parse(source); \
    EXPECT_FALSE(errors.empty());     \
}

#define PARSE_EXPECT_NERROR(source, n) \
{                                      \
    auto [_, errors] = parse(source);  \
    EXPECT_EQ(errors.size(), n);       \
}

class ParserTest : public ::testing::Test {
protected:
    AST ast;

    void SetUp() override {
        ast = AST();
    }
};

TEST_F(ParserTest, LambdaTest1) {
    PARSE_EXPECT("fn (x, y) => x + y",
        BLOCK(
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
        )
    );
}

TEST_F(ParserTest, LambdaTest2) {
    PARSE_EXPECT("fn^ (x: Int, y: Int) -> Int => x * y",
        BLOCK(
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
        )
    );
}

TEST_F(ParserTest, LambdaTest3) {
    PARSE_EXPECT("fn (x)[ref y] => x + y",
        BLOCK(
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
        )
    );
}

TEST_F(ParserTest, LambdaTest4) {
    PARSE_EXPECT("fn (x) throw Error => risky(x)",
        BLOCK(
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
        )
    );
}

TEST_F(ParserTest, LambdaTest5) {
    PARSE_EXPECT("fn (x: Int) -> Bool => x % 2 == 0",
        BLOCK(
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
        )
    );
}

TEST_F(ParserTest, BlockTest1) {
    PARSE_EXPECT("{ }",
        BLOCK(
            BLOCK()
        )
    );
}

TEST_F(ParserTest, BlockTest2) {
    PARSE_EXPECT("{ a; b; c }",
        BLOCK(
            BLOCK(
                IDENT("a"),
                IDENT("b"),
                IDENT("c")
            )
        )
    );
}

TEST_F(ParserTest, DISABLED_BlockTest3) {
    PARSE_EXPECT(R"(
{
    a
    b
    c
}
)",
        BLOCK(
            BLOCK(
                IDENT("a"),
                IDENT("b"),
                IDENT("c")
            )
        )
    );
}

TEST_F(ParserTest, IfThenTest1) {
    PARSE_EXPECT("if foo() then a else b",
        BLOCK(
            IFTHEN(
                CALL(IDENT("foo")), IDENT("a"), IDENT("b")
            )
        )
    );
}

TEST_F(ParserTest, TryElseTest1) {
    PARSE_EXPECT("x = try { foo(); throw Error() } else 1",
        BLOCK(
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
        )
    );
}

TEST_F(ParserTest, MatchTest1) {
    PARSE_EXPECT("match x { case y => z }",
        BLOCK(
            MATCH(
               IDENT("x"),
                MATCH_CASE(
                    MAKE(BindingPattern, NAME("y")),
                        /* guard */ nullptr,
                        IDENT("z")
                )
            )
        )
    );
}

TEST_F(ParserTest, MatchTest2) {
    PARSE_EXPECT("match x { case if cond => y }",
        BLOCK(
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
}

TEST_F(ParserTest, MatchTest3) {
    PARSE_EXPECT("match x { case y if cond => z }",
        BLOCK(
            MATCH(
                IDENT("x"),
                MATCH_CASE(
                    MAKE(BindingPattern, NAME("y")),
                    IDENT("cond"),
                    IDENT("z")
                )
            )
        )
    );
}

TEST_F(ParserTest, MatchTest4) {
    PARSE_EXPECT(R"(
match x {
    case a => b
    case c if d => e
}
)",
        BLOCK(
            MATCH(
                IDENT("x"),
                MATCH_CASE(BIND_PAT(NAME("a")), nullptr, IDENT("b")),
                MATCH_CASE(BIND_PAT(NAME("c")), IDENT("d"), IDENT("e"))
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest1) {
    // Making sure `*` binds tighter than `+`
    PARSE_EXPECT("a + b * c",
        BLOCK(
            BINARY(BINARY_OP::Add,
                IDENT("a"), BINARY(BINARY_OP::Mul, IDENT("b"), IDENT("c"))
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest2) {
    // Making sure `*` binds tighter than `+`
    PARSE_EXPECT("a * b + c * d",
        BLOCK(
            BINARY(BINARY_OP::Add,
                BINARY(BINARY_OP::Mul, IDENT("a"), IDENT("b")),
                BINARY(BINARY_OP::Mul, IDENT("c"), IDENT("d")))
        )
    );
}

TEST_F(ParserTest, OperatorTest3) {
    // Making sure arithmetic of the same operator is left-associative
    PARSE_EXPECT("a - b - c",
        BLOCK(
            BINARY(BINARY_OP::Sub,
                BINARY(BINARY_OP::Sub, IDENT("a"), IDENT("b")),
                IDENT("c")
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest4) {
    // Making sure prefix binds tighter than binary
    PARSE_EXPECT("-a * b",
        BLOCK(
            BINARY(BINARY_OP::Mul,
                PREFIX(PREFIX_OP::Neg, IDENT("a")),
                IDENT("b")
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest5) {
    // Making sure prefix binds tighter than binary
    PARSE_EXPECT("-a + b",
        BLOCK(
            BINARY(BINARY_OP::Add,
               PREFIX(PREFIX_OP::Neg, IDENT("a")),
               IDENT("b")
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest6) {
    // Making sure expression in parentheses binds tighter than others
    PARSE_EXPECT("(a + b) * c",
        BLOCK(
            BINARY(BINARY_OP::Mul,
               BINARY(BINARY_OP::Add, IDENT("a"), IDENT("b")),
               IDENT("c")
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest7) {
    // Making sure comparison binds looser than arithmetic
    PARSE_EXPECT("a + b < c * d",
        BLOCK(
            BINARY(BINARY_OP::Lt,
                BINARY(BINARY_OP::Add, IDENT("a"), IDENT("b")),
                BINARY(BINARY_OP::Mul, IDENT("c"), IDENT("d"))
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest8) {
    // Making sure `&&` binds tighter than `||`
    PARSE_EXPECT("a || b && c",
        BLOCK(
            BINARY(BINARY_OP::LogOr,
                IDENT("a"),
                BINARY(BINARY_OP::LogAnd, IDENT("b"), IDENT("c"))
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest9) {
    // Binary operator stress test
    PARSE_EXPECT("a + b * c < d && e || f",
        BLOCK(
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
}

TEST_F(ParserTest, OperatorTest10) {
    // Making sure comparison is non-associative
    PARSE_EXPECT_ERROR("a < b < c");
}

TEST_F(ParserTest, OperatorTest11) {
    // Making sure equality is non-associative
    PARSE_EXPECT_ERROR("a == b === c");
}

TEST_F(ParserTest, OperatorTest12) {
    PARSE_EXPECT("-~!a",
        BLOCK(
            PREFIX(PREFIX_OP::Neg,
                PREFIX(PREFIX_OP::BitNot,
                    PREFIX(PREFIX_OP::LogNot, IDENT("a"))
                )
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest13) {
    // Make sure postfix binds tighter than prefix
    PARSE_EXPECT("-a!",
        BLOCK(
            PREFIX(PREFIX_OP::Neg,
               POSTFIX(POSTFIX_OP::ForceUnwrap, IDENT("a"))
            )
        )
    );
}

TEST_F(ParserTest, OperatorTest14) {
    // Make sure postfix binds tighter than prefix
    PARSE_EXPECT("!!!a!!!",
        BLOCK(
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
}

TEST_F(ParserTest, OperatorTest15) {
    PARSE_EXPECT("a.b(c)[d]",
        BLOCK(
            SUBSCRIPT(
                CALL(
                    MEMBER(IDENT("a"), NAME("b")),
                    CALL_ARG(IDENT("c"))
                ),
                IDENT("d")
            )
        )
    );
}

TEST_F(ParserTest, TypeofTest1) {
    PARSE_EXPECT("typeof(x + y)",
        BLOCK(
            MAKE(TypeofExpr,
                 BINARY(BINARY_OP::Add, IDENT("x"), IDENT("y"))
            )
        )
    );
}

TEST_F(ParserTest, VarDefTest1) {
    PARSE_EXPECT("let x: T = y",
        BLOCK(
            VARDEF(
                VARMOD(VARKIND::Let, false, VAROPT::None),
                BIND_PAT(NAME("x")),
                IDENT("T"),
                IDENT("y")
            )
        )
    );
}

TEST_F(ParserTest, VarDefTest2) {
    PARSE_EXPECT("const^ x: T = y",
        BLOCK(
            VARDEF(
                VARMOD(VARKIND::Const, true, VAROPT::None),
                MAKE(BindingPattern, NAME("x")),
                IDENT("T"),
                IDENT("y")
            )
        )
    );
}

TEST_F(ParserTest, VarDefTest3) {
    PARSE_EXPECT("let? x: T",
        BLOCK(
            VARDEF(
                VARMOD(VARKIND::Let, false, VAROPT::Optional),
                MAKE(BindingPattern, NAME("x")),
                IDENT("T")
            )
        )
    );
}

TEST_F(ParserTest, VarDefTest4) {
    PARSE_EXPECT("ref x = y",
        BLOCK(
            VARDEF(
                VARMOD(VARKIND::Ref, false, VAROPT::None),
                MAKE(BindingPattern, NAME("x")),
                nullptr,
                IDENT("y")
            )
        )
    );
}

TEST_F(ParserTest, FnDefTest1) {
    PARSE_EXPECT("fn foo() { }",
        BLOCK(
            MAKE(FnDefStmt,
                /* isImmutable */ false,
                NAME("foo"),
                /* generics */ std::vector<Name*>(),
                /* params */ std::vector<FnParam*>(),
                /* captureClause */ nullptr,
                /* returns */ std::vector<FnReturn*>(),
                /* isThrowing */ false,
                /* throwExpr */ nullptr,
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, FnDefTest2) {
    PARSE_EXPECT("fn^ foo[T]() { }",
        BLOCK(
            MAKE(FnDefStmt,
                /* isImmutable */ true,
                NAME("foo"),
                /* generics */ std::vector<Name*>{ NAME("T") },
                /* params */ std::vector<FnParam*>(),
                /* captureClause */ nullptr,
                /* returns */ std::vector<FnReturn*>(),
                /* isThrowing */ false,
                /* throwExpr */ nullptr,
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, FnDefTest3) {
    PARSE_EXPECT("fn foo(x, y) { }",
        BLOCK(
            MAKE(FnDefStmt,
                /* isImmutable */ false,
                NAME("foo"),
                /* generics */ std::vector<Name*>(),
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
        )
    );
}

TEST_F(ParserTest, FnDefTest4) {
    PARSE_EXPECT("fn foo()[] { }",
        BLOCK(
            MAKE(FnDefStmt,
                /* isImmutable */ false,
                NAME("foo"),
                /* generics */ std::vector<Name*>(),
                /* params */ std::vector<FnParam*>(),
                MAKE(FnCaptureClause,
                    std::vector<FnCapture*>(), false, nullptr
                ),
                /* returns */ std::vector<FnReturn*>(),
                /* isThrowing */ false,
                /* throwExpr */ nullptr,
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, FnDefTest5) {
    PARSE_EXPECT("fn foo() -> Int { }",
        BLOCK(
            MAKE(FnDefStmt,
                /* isImmutable */ false,
                NAME("foo"),
                /* generics */ std::vector<Name*>(),
                /* params */ std::vector<FnParam*>(),
                /* captureClause */ nullptr,
                std::vector<FnReturn*>{
                    MAKE(FnReturn, FnReturn::RetKind::ByValue, IDENT("Int"))
                },
                /* isThrowing */ false,
                /* throwExpr */ nullptr,
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, FnDefTest6) {
    PARSE_EXPECT("fn foo(x) => x",
        BLOCK(
            MAKE(FnDefStmt,
                /* isImmutable */ false,
                NAME("foo"),
                /* generics */ std::vector<Name*>(),
                std::vector<FnParam*>{
                    MAKE(FnParam, nullptr, BIND_PAT(NAME("x")), nullptr, nullptr)
                },
                /* captureClause */ nullptr,
                /* returns */ std::vector<FnReturn*>(),
                /* isThrowing */ false,
                /* throwExpr */ nullptr,
                IDENT("x")
            )
        )
    );
}

TEST_F(ParserTest, FnDefTest7) {
    PARSE_EXPECT("fn foo() throw Error { }",
        BLOCK(
            MAKE(FnDefStmt,
                /* isImmutable */ false,
                NAME("foo"),
                /* generics */ std::vector<Name*>(),
                /* params */ std::vector<FnParam*>(),
                /* captureClause */ nullptr,
                /* returns */ std::vector<FnReturn*>(),
                /* isThrowing */ true,
                /* throwExpr */ IDENT("Error"),
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, TypeDefTest1) {
    PARSE_EXPECT("struct Foo { }",
        BLOCK(
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::Struct,
                /* isImmutable */ false,
                NAME("Foo"),
                /* template */ std::vector<Name*>(),
                /* bases */ std::vector<Expr*>(),
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, TypeDefTest2) {
    PARSE_EXPECT("class^ Foo { }",
        BLOCK(
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::Class,
                /* isImmutable */ true,
                NAME("Foo"),
                /* template */ std::vector<Name*>(),
                /* bases */ std::vector<Expr*>(),
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, TypeDefTest3) {
    PARSE_EXPECT("struct Foo : Bar, Baz { }",
        BLOCK(
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::Struct,
                /* isImmutable */ false,
                NAME("Foo"),
                /* template */ std::vector<Name*>(),
                /* bases */ std::vector<Expr*>{
                    IDENT("Bar"),
                    IDENT("Baz")
                },
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, TypeDefTest4) {
    PARSE_EXPECT("enum class Color { }",
        BLOCK(
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::EnumClass,
                /* isImmutable */ false,
                NAME("Color"),
                /* generics */ std::vector<Name*>(),
                /* bases */ std::vector<Expr*>(),
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, TypeDefTest5) {
    PARSE_EXPECT("struct Foo[T, U] { }",
        BLOCK(
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::Struct,
                /* isImmutable */ false,
                NAME("Foo"),
                std::vector<Name*>{
                    NAME("T"),
                    NAME("U")
                },
                /* bases */ std::vector<Expr*>(),
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, CaseDefTest1) {
    PARSE_EXPECT(R"(
enum Color {
    case Red;
    case Yellow;
    case Blue;
}
)",
        BLOCK(
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::Enum,
                /* isImmutable */ false,
                NAME("Color"),
                /* generics */ std::vector<Name*>(),
                /* bases */ std::vector<Expr*>(),
                BLOCK(
                    MAKE(CaseDefStmt, NAME("Red")),
                    MAKE(CaseDefStmt, NAME("Yellow")),
                    MAKE(CaseDefStmt, NAME("Blue")),
                )
            )
        )
    );
}

TEST_F(ParserTest, CaseDefTest2) {
    PARSE_EXPECT(R"(
enum OpKind : UInt32 {
    case None = 0;
    case Add  = 1;
    case Sub  = 2;
    case Mul  = 3;
    case Div  = 4;
}
)",
        BLOCK(
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::Enum,
                /* isImmutable */ false,
                NAME("OpKind"),
                /* generics */ std::vector<Name*>(),
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
        )
    );
}

TEST_F(ParserTest, AssignTest1) {
    // Making sure `??=` binds looser than others
    PARSE_EXPECT("a = b ?\?= c",
        BLOCK(
            ASSIGN(ASSIGN_OP::CoalesceAssign,
                ASSIGN(ASSIGN_OP::Assign, IDENT("a"), IDENT("b")),
            IDENT("c")
            )
        )
    );
}

TEST_F(ParserTest, AssignTest2) {
    // Making sure assignment is right-associative
    PARSE_EXPECT("a = b += c",
        BLOCK(
            ASSIGN(ASSIGN_OP::Assign,
                IDENT("a"),
                ASSIGN(ASSIGN_OP::AddAssign, IDENT("b"), IDENT("c"))
            )
        )
    );
}

TEST_F(ParserTest, AssignTest3) {
    // Making sure assignment is right-associative
    PARSE_EXPECT("a = b += c *= d",
        BLOCK(
            ASSIGN(ASSIGN_OP::Assign,
                IDENT("a"),
                ASSIGN(ASSIGN_OP::AddAssign,
                    IDENT("b"),
                    ASSIGN(ASSIGN_OP::MulAssign, IDENT("c"), IDENT("d"))
                )
            )
        )
    );
}

TEST_F(ParserTest, IfStmtTest1) {
    PARSE_EXPECT("if a { b }",
        BLOCK(
            MAKE(IfStmt,
                IDENT("a"),
                BLOCK(
                    IDENT("b")
                ),
                /* elseBody */ nullptr
            )
        )
    );
}

TEST_F(ParserTest, IfStmtTest2) {
    PARSE_EXPECT("if a { b } else { c }",
        BLOCK(
            MAKE(IfStmt,
                IDENT("a"),
                BLOCK(
                    IDENT("b")
                ),
                BLOCK(
                    IDENT("c")
                )
            )
        )
    );
}

TEST_F(ParserTest, IfStmtTest3) {
    PARSE_EXPECT("if a { b } else if c { d }",
        BLOCK(
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
        )
    );
}

TEST_F(ParserTest, IfStmtTest4) {
    PARSE_EXPECT("if a { b } else if c { d } else { e }",
        BLOCK(
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
        )
    );
}

TEST_F(ParserTest, WhileStmtTest1) {
    PARSE_EXPECT("while a { b }",
        BLOCK(
            MAKE(WhileStmt,
                IDENT("a"),
                BLOCK(
                    IDENT("b")
                )
            )
        )
    );
}

TEST_F(ParserTest, DoWhileStmtTest1) {
    PARSE_EXPECT("do { a } while b",
        BLOCK(
            MAKE(DoWhileStmt,
                BLOCK(
                    IDENT("a")
                ),
                IDENT("b")
            )
        )
    );
}

TEST_F(ParserTest, ForStmtTest1) {
    PARSE_EXPECT("for i in 0...10 { print(i) }",
        BLOCK(
            MAKE(ForStmt,
                BIND_PAT(NAME("i")),
                BINARY(BINARY_OP::Range, INT(0), INT(10)),
                BLOCK(
                    CALL(IDENT("print"), CALL_ARG(IDENT("i")))
                )
            )
        )
    );
}

TEST_F(ParserTest, ForStmtTest2) {
    PARSE_EXPECT("for (x, y) in tuples { foo(x, y) }",
        BLOCK(
            MAKE(ForStmt,
                TUP_PAT(BIND_PAT(NAME("x")), BIND_PAT(NAME("y"))),
                IDENT("tuples"),
                BLOCK(
                    CALL(IDENT("foo"), CALL_ARG(IDENT("x")), CALL_ARG(IDENT("y")))
                )
            )
        )
    );
}

TEST_F(ParserTest, ForStmtTest3) {
    PARSE_EXPECT("for i in [1, 2, 3] { y += x }",
        BLOCK(
            MAKE(ForStmt,
                BIND_PAT(NAME("i")),
                COLLECTION(INT(1), INT(2), INT(3)),
                BLOCK(
                    ASSIGN(ASSIGN_OP::AddAssign, IDENT("y"), IDENT("x"))
                )
            )
        )
    );
}

TEST_F(ParserTest, ForStmtTest4) {
    PARSE_EXPECT("for j in -10..<10 { print('value: ' + j) }",
        BLOCK(
            MAKE(ForStmt,
                BIND_PAT(NAME("j")),
                BINARY(BINARY_OP::RangeExcl, PREFIX(PREFIX_OP::Neg, INT(10)), INT(10)),
                BLOCK(
                    CALL(IDENT("print"), CALL_ARG(BINARY(BINARY_OP::Add, STRING("value: "), IDENT("j"))))
                )
            )
        )
    );
}

TEST_F(ParserTest, ForStmtTest5) {
    PARSE_EXPECT(R"(
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
)",
        BLOCK(
            MAKE(FnDefStmt,
                /* isImmutable */ false,
                NAME("findFirstEven"),
                /* generics */ std::vector<Name*>(),
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
        )
    );
}

TEST_F(ParserTest, ModuleStmtTest1) {
    PARSE_EXPECT("module Foo { }",
        BLOCK(
            MAKE(ModuleStmt,
                PATH(PATH_SEG(NAME("Foo"))),
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, ModuleStmtTest2) {
    PARSE_EXPECT("module Foo.Bar { }",
        BLOCK(
            MAKE(ModuleStmt,
                PATH(PATH_SEG(NAME("Foo")), PATH_SEG(NAME("Bar"))),
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, ModuleStmtTest3) {
    PARSE_EXPECT("module Foo.Bar.Baz { }",
        BLOCK(
            MAKE(ModuleStmt,
                PATH(PATH_SEG(NAME("Foo")), PATH_SEG(NAME("Bar")), PATH_SEG(NAME("Baz"))),
                BLOCK()
            )
        )
    );
}

TEST_F(ParserTest, ExportStmtTest1) {
    PARSE_EXPECT(
R"(
export module Foo {
    export let x: Bar = Bar()
}
)",
        BLOCK(
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
        )
    );
}

TEST_F(ParserTest, ExportStmtTest2) {
    PARSE_EXPECT(
R"(
export fn^ max[T](a: T, b: T) -> T throw Error {
    return myMax(a, b)
}
)",
        BLOCK(
            MAKE(ExportStmt,
                MAKE(FnDefStmt,
                    /* isImmutable */ true,
                    NAME("max"),
                    /* generics */ std::vector<Name*>{
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
        )
    );
}

TEST_F(ParserTest, ExportStmtTest3) {
    PARSE_EXPECT(
R"(
export class Box[T] : Container { }
)",
        BLOCK(
            MAKE(ExportStmt,
                MAKE(TypeDefStmt,
                    TypeDefStmt::TypeKind::Class,
                    /* isImmutable */ false,
                    NAME("Box"),
                    /* generics */ std::vector<Name*>{
                        NAME("T")
                    },
                    /* bases */ std::vector<Expr*>{
                        IDENT("Container")
                    },
                    BLOCK()
                )
            )
        )
    );
}

TEST_F(ParserTest, ExportTypeDefTest2) {
    PARSE_EXPECT(
R"(
export class Box[T] : Container {
    let value: T;

    fn get() -> T {
        return value;
    }
}
)",
        BLOCK(
            MAKE(ExportStmt,
                MAKE(TypeDefStmt,
                    TypeDefStmt::TypeKind::Class,
                    /* isImmutable */ false,
                    NAME("Box"),
                    /* generics */ std::vector<Name*>{
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
                            /* generics */ std::vector<Name*>(),
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
        )
    );
}

TEST_F(ParserTest, ImportStmtTest1) {
    PARSE_EXPECT("from Foo import Bar",
        BLOCK(
            MAKE(ImportStmt,
                PATH(PATH_SEG(NAME("Foo"))),
                std::vector<ImportItem*>{
                    MAKE(ImportItem,
                        PATH(PATH_SEG(NAME("Bar"))),
                        nullptr
                    )
                }
            )
        )
    );
}

TEST_F(ParserTest, ImportStmtTest2) {
    PARSE_EXPECT("from Foo import Bar, Baz as Qux",
        BLOCK(
            MAKE(ImportStmt,
                PATH(PATH_SEG(NAME("Foo"))),
                std::vector<ImportItem*>{
                    MAKE(ImportItem,
                        PATH(PATH_SEG(NAME("Bar"))),
                        nullptr
                    ),
                    MAKE(ImportItem,
                        PATH(PATH_SEG(NAME("Baz"))),
                        NAME("Qux")
                    )
                }
            )
        )
    );
}

TEST_F(ParserTest, ImportStmtTest3) {
    PARSE_EXPECT("from Foo.Bar[Int] import Baz[Bool] as BazBool, Qux",
        BLOCK(
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
                        nullptr
                    )
                }
            )
        )
    );
}

TEST_F(ParserTest, ImportAllStmtTest1) {
    PARSE_EXPECT("from Foo import *",
        BLOCK(
            MAKE(ImportAllStmt,
                PATH(PATH_SEG(NAME("Foo")))
            )
        )
    );
}

TEST_F(ParserTest, ImportAllStmtTest2) {
    PARSE_EXPECT("from Foo.Bar[Int] import *",
        BLOCK(
            MAKE(ImportAllStmt,
                PATH(PATH_SEG(NAME("Foo")), PATH_SEG(NAME("Bar"), IDENT("Int")))
            )
        )
    );
}

TEST_F(ParserTest, ImportAllStmtTest3) {
    PARSE_EXPECT("from Foo.Bar[Int].Baz[Bool] import *",
        BLOCK(
            MAKE(ImportAllStmt,
                PATH(
                    PATH_SEG(NAME("Foo")),
                    PATH_SEG(NAME("Bar"), IDENT("Int")),
                    PATH_SEG(NAME("Baz"), IDENT("Bool"))
                )
            )
        )
    );
}

TEST_F(ParserTest, UndefineStmtTest1) {
    PARSE_EXPECT("undefine Foo",
        BLOCK(
            MAKE(UndefineStmt,
                PATH(PATH_SEG(NAME("Foo")))
            )
        )
    );
}

TEST_F(ParserTest, UndefineStmtTest2) {
    PARSE_EXPECT("undefine Foo.Bar[Int]",
        BLOCK(
            MAKE(UndefineStmt,
                PATH(
                    PATH_SEG(NAME("Foo")),
                    PATH_SEG(NAME("Bar"), IDENT("Int"))
                )
            )
        )
    );
}

TEST_F(ParserTest, UndefineStmtTest3) {
    PARSE_EXPECT("undefine Foo.Bar[Int].constructor",
        BLOCK(
            MAKE(UndefineStmt,
                PATH(
                    PATH_SEG(NAME("Foo")),
                    PATH_SEG(NAME("Bar"), IDENT("Int")),
                    PATH_SEG(CONSTRUCTOR)
                )
            )
        )
    );
}

TEST_F(ParserTest, Test1) {
    PARSE_EXPECT(
R"(
fn fact(n: Int) -> Int {
    if n <= 1 {
        return 1;
    };

    return n * fact(n - 1);
}
)",
        BLOCK(
            MAKE(FnDefStmt,
                false,
                NAME("fact"),
                std::vector<Name*>(),
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
        )
    );
}

TEST_F(ParserTest, Test2) {
    PARSE_EXPECT(
R"(
fn fib(n: Int) -> Int {
    if n <= 1 {
        return n
    };

    return fib(n - 1) + fib(n - 2)
}
)",
        BLOCK(
            MAKE(FnDefStmt,
                false,
                NAME("fib"),
                std::vector<Name*>(),
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
        )
    );
}

TEST_F(ParserTest, Test3) {
    PARSE_EXPECT(
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
)",
        BLOCK(
            MAKE(FnDefStmt,
                false,
                NAME("firstEven"),
                std::vector<Name*>(),
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
        )
    );
}

TEST_F(ParserTest, Test4) {
    PARSE_EXPECT(
R"(
fn makeAdder(base: Int) -> Int -> Int {
    return fn (x) [base] => x + base;
}
)",
        BLOCK(
            MAKE(FnDefStmt,
                false,
                NAME("makeAdder"),
                std::vector<Name*>(),
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
        )
    );
}

TEST_F(ParserTest, Test5) {
    PARSE_EXPECT(
R"(
struct Counter {
    let value: Int;

    fn inc() -> Int {
        return value + 1;
    }
}
)",
        BLOCK(
            MAKE(TypeDefStmt,
                TypeDefStmt::TypeKind::Struct,
                false,
                NAME("Counter"),
                std::vector<Name*>(),
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
                        std::vector<Name*>(),
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
        )
    );
}

TEST_F(ParserTest, TestA) {
    PARSE_EXPECT_SUCCESS(
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
}

TEST_F(ParserTest, TestB) {
    PARSE_EXPECT_SUCCESS(
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
}

TEST_F(ParserTest, TestC) {
    PARSE_EXPECT_SUCCESS(
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
}

TEST_F(ParserTest, TestD) {
    PARSE_EXPECT_SUCCESS(
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
}

TEST_F(ParserTest, TestE) {
    PARSE_EXPECT_SUCCESS(
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
}

TEST_F(ParserTest, TestF) {
    PARSE_EXPECT_SUCCESS(
R"(
fn potsOfGold(pots: ^Array[Int]) -> Int {
    fn recursion(pots: ^Array[Int], i: Int, j: Int, opt: ^Array[^Array[Int]]) -> Int {
        if i > j {
            return 0;
        };

        if opt[i][j] != 0 {
            return opt[i][j];
        };

        if i == j {
            opt[i][j] = pots[i];
            return opt[i][j];
        };

        const pickLeft =
            pots[i] + min(
                recursion(pots, i + 2, j, opt),
                recursion(pots, i + 1, j - 1, opt)
            );

        const pickRight =
            pots[j] + min(
                recursion(pots, i, j - 2, opt),
                recursion(pots, i + 1, j - 1, opt)
            );

        opt[i][j] = max(pickLeft, pickRight);
        return opt[i][j];
    };

    const n = pots.size();
    const opt: ^Array[^Array[Int]] = Array(n);

    for i in 0...n {
        opt[i] = Array(n);
        for j in 0...n {
            opt[i][j] = 0;
        };
    };

    return recursion(pots, 0, n - 1, opt);
};

print(potsOfGold([10, 5, 15, 20]));
)");
}

TEST_F(ParserTest, TestG) {
    PARSE_EXPECT_SUCCESS(
 R"(
fn longestIncreasingPath(A, m, n) {
    const OPT = Array(m);
    for i in 0...m {
        OPT[i] = Array(n);
        for j in 0...n {
            OPT[i][j] = nil;
        };
    };

    let longest = [];

    fn recursion(A, m, n, prev, i, j, OPT) {
        if i < 0 || j < 0 || i >= m || j >= n || A[i][j] <= prev {
            return []
        };

        if OPT[i][j] != nil {
            return OPT[i][j]
        };

        const up = recursion(A, m, n, A[i][j], i, j - 1, OPT);
        const down = recursion(A, m, n, A[i][j], i, j + 1, OPT);
        const left = recursion(A, m, n, A[i][j], i - 1, j, OPT);
        const right = recursion(A, m, n, A[i][j], i + 1, j, OPT);

        let best = up;
        if down.size() > best.size() {
            best = down
        };
        if left.size() > best.size() {
            best = left
        };
        if right.size() > best.size() {
            best = right
        };

        OPT[i][j] = [A[i][j]] + best;
        return OPT[i][j];
    };

    for i in 0...m {
        for j in 0...n {
            const path = recursion(A, m, n, 0, i, j, OPT);
            if path.size() > longest.size() {
                longest = path;
            };
        }
    };

    return longest;
}
)");
}

TEST_F(ParserTest, TestH) {
    PARSE_EXPECT_SUCCESS(
 R"(
fn maxProfit(i, p) {
    const OPT = Array(i + 1);
    for k in 0...i {
        OPT[k] = nil;
    };

    fn recursion(i, p, OPT) {
        if OPT[i] != nil {
            return OPT[i];
        };

        if i == 0 {
            OPT[i] = 0;
            return 0;
        };

        let profit = p(i);

        for j in 1...i {
            const candidate =
                recursion(j, p, OPT) + recursion(i - j, p, OPT);

            if candidate > profit {
                profit = candidate;
            };
        };

        OPT[i] = profit;
        return profit;
    };

    return recursion(i, p, OPT);
};
)");
}

TEST_F(ParserTest, TestI) {
    PARSE_EXPECT_SUCCESS(
 R"(
fn numberOfShortestPaths(n, m) {
    const OPT = Array(n + 1);
    for i in 0...n {
        OPT[i] = Array(m + 1);
        for j in 0...m {
            OPT[i][j] = nil;
        };
    };

    fn recursion(n, m, OPT) {
        if OPT[n][m] != nil {
            return OPT[n][m];
        };

        if n == 0 && m == 0 {
            OPT[n][m] = 1;
        } else if n == 0 {
            OPT[n][m] = recursion(n, m - 1, OPT);
        } else if m == 0 {
            OPT[n][m] = recursion(n - 1, m, OPT);
        } else {
            OPT[n][m] = recursion(n, m - 1, OPT) + recursion(n - 1, m, OPT);
        };

        return OPT[n][m];
    };

    return recursion(n, m, OPT);
};
)");
}

TEST_F(ParserTest, TestJ) {
    PARSE_EXPECT_SUCCESS(
 R"(
fn paveWays(n) {
    const OPT = Array(n + 1);
    for i in 0...n {
        OPT[i] = nil;
    };

    fn pave(n, OPT) {
        if OPT[n] != nil {
            return OPT[n];
        };

        if n < 2 {
            OPT[n] = 0;
        } else if n == 2 {
            OPT[n] = 1;
        } else if n == 3 {
            OPT[n] = 1;
        } else if n == 5 {
            OPT[n] = 3;
        } else {
            OPT[n] = pave(n - 2, OPT) + pave(n - 3, OPT) + pave(n - 5, OPT);
        };

        return OPT[n];
    };

    return pave(n, OPT);
};
)");
}
