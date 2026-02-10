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

#define RESOLVE_EXPECT_SUCCESS(source, symTable, nodeSymMap, globalEnv)                           \
{                                                                                                 \
    AST ast = PARSE(source);                                                                      \
    NameResolveResult result = NameResolver::resolve(ast, (symTable), (nodeSymMap), (globalEnv)); \
    EXPECT_FALSE(result.diagnostics.hasError()) << result.diagnostics << "\n";                    \
}

#define RESOLVE_EXPECT_FAIL(source, symTable, nodeSymMap, globalEnv)                              \
{                                                                                                 \
    AST ast = PARSE(source);                                                                      \
    NameResolveResult result = NameResolver::resolve(ast, (symTable), (nodeSymMap), (globalEnv)); \
    EXPECT_TRUE(result.diagnostics.hasError());                                                   \
}

TEST(NameResolverTest, Valid1) {
    std::string_view source = R"(
        let x = 1;
        let y = x + 1;
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_SUCCESS(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Valid2) {
    // Shadowing
    std::string_view source = R"(
        let x = 1;
        {
            let x = 2;
            let y = x;
        }
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_SUCCESS(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Valid3) {
    // Function is hoisted
    std::string_view source = R"(
        foo();
        fn foo() { };
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_SUCCESS(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Valid4) {
    std::string_view source = R"(
        let x = 1;
        {
            let y = x;
        }
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_SUCCESS(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Valid5) {
    std::string_view source = R"(
        let a = 1;
        {
            let b = a;
            {
                let c = b;
            }
        }
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_SUCCESS(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Invalid1) {
    // Undefined symbol `print` and `x`
    std::string_view source = R"(
        print(x);
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_FAIL(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Invalid2) {
    // Undefined symbol `x`
    std::string_view source = R"(
        let y = x + 1;
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_FAIL(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Invalid3) {
    // Undefined symbol `x`
    std::string_view source = R"(
        {
            let x = 1;
        };
        let y = x;
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_FAIL(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Invalid4) {
    // Redeclaration of `x`
    std::string_view source = R"(
        let x = 1;
        let x = 2;
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_FAIL(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Invalid5) {
    std::string_view source = R"(
        let a = 1;
        {
            let b = a;
            {
                let c = d;
            }
        }
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_FAIL(source, symTable, nodeSymMap, globalEnv);
}

TEST(NameResolverTest, Invalid6) {
    // Use before declaration
    std::string_view source = R"(
        let y = x + 1;
        let x = 2;
    )";
    SymbolTable symTable;
    NodeSymbolMap nodeSymMap;
    Env globalEnv;
    RESOLVE_EXPECT_FAIL(source, symTable, nodeSymMap, globalEnv);
}
