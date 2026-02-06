#include <sstream>

#include <gtest/gtest.h>

#include "frontend/parser.hpp"
#include "frontend/semantic/name_resolver.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

#define PARSE(source)                                                             \
({                                                                                \
    std::istringstream iss{std::string(source)};                                  \
    ParseResult result = Parser::parse(iss);                                      \
    if (result.diagnostics.hasError()) {                                          \
        std::ostringstream oss;                                                   \
        for (const Diagnostic& d : result.diagnostics.diagnostics()) {            \
            oss << d << "\n";                                                     \
        }                                                                         \
        FAIL() << "error when parsing source: \n" << source << "\n" << oss.str(); \
    }                                                                             \
    std::move(result.ast);                                                        \
})

#define RESOLVE_EXPECT_SUCCESS(source, symTable)                     \
{                                                                    \
    AST ast = PARSE(source);                                         \
    NameResolveResult result = NameResolver::resolve(ast, symTable); \
    EXPECT_FALSE(result.diagnostics.hasError());                     \
}

#define RESOLVE_EXPECT_FAIL(source, symTable)                        \
{                                                                    \
    AST ast = PARSE(source);                                         \
    NameResolveResult result = NameResolver::resolve(ast, symTable); \
    EXPECT_TRUE(result.diagnostics.hasError());                      \
}

TEST(NameResolverTest, Valid1) {
    AST ast = PARSE(R"(
        let x = 1;
        let y = x + 1;
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_FALSE(result.diagnostics.hasError());
}

TEST(NameResolverTest, Valid2) {
    // Shadowing
    AST ast = PARSE(R"(
        let x = 1;
        {
            let x = 2;
            let y = x;
        }
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_FALSE(result.diagnostics.hasError());
}

TEST(NameResolverTest, Valid3) {
    // Function is hoisted
    AST ast = PARSE(R"(
        foo();
        fn foo() { };
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_FALSE(result.diagnostics.hasError());
}

TEST(NameResolverTest, Valid4) {
    AST ast = PARSE(R"(
        let x = 1;
        {
            let y = x;
        }
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_FALSE(result.diagnostics.hasError());
}

TEST(NameResolverTest, Valid5) {
    AST ast = PARSE(R"(
        let a = 1;
        {
            let b = a;
            {
                let c = b;
            }
        }
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_FALSE(result.diagnostics.hasError());
}

TEST(NameResolverTest, Invalid1) {
    // Undefined symbol `print` and `x`
    AST ast = PARSE(R"(
        print(x);
)");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.hasError());
}

TEST(NameResolverTest, Invalid2) {
    // Undefined symbol `x`
    AST ast = PARSE(R"(
        let y = x + 1;
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.hasError());
}

TEST(NameResolverTest, Invalid3) {
    // Undefined symbol `x`
    AST ast = PARSE(R"(
        {
            let x = 1;
        };
        let y = x;
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.hasError());
}

TEST(NameResolverTest, Invalid4) {
    // Redeclaration of `x`
    AST ast = PARSE(R"(
        let x = 1;
        let x = 2;
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.hasError());
}

TEST(NameResolverTest, Invalid5) {
    AST ast = PARSE(R"(
        let a = 1;
        {
            let b = a;
            {
                let c = d;
            }
        }
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.hasError());
}
