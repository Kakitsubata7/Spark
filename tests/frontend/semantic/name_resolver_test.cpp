#include <gtest/gtest.h>

#include "frontend/parser.hpp"
#include "frontend/semantic/name_resolver.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

static AST parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    auto [ast, errors] = Parser::parse(iss);
    EXPECT_TRUE(errors.empty()) << "error during parsing source: \n" << source;
    return std::move(ast);
}

TEST(NameResolverTest, Valid1) {
    AST ast = parse(R"(
        let x = 1;
        let y = x + 1;
)");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.empty());
}

TEST(NameResolverTest, Valid2) {
    // Shadowing
    AST ast = parse(R"(
        let x = 1;
        {
            let x = 2;
            let y = x;
        }
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.empty());
}

TEST(NameResolverTest, Valid3) {
    // Function is hoisted
    AST ast = parse(R"(
        foo();
        fn foo() { };
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.empty());
}

TEST(NameResolverTest, Valid4) {
    AST ast = parse(R"(
        let x = 1;
        {
            let y = x;
        }
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.empty());
}

TEST(NameResolverTest, Valid5) {
    AST ast = parse(R"(
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
    EXPECT_TRUE(result.diagnostics.empty());
}

TEST(NameResolverTest, Invalid1) {
    // Undefined symbol `print` and `x`
    AST ast = parse(R"(
        print(x);
)");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_FALSE(result.diagnostics.empty());
}

TEST(NameResolverTest, Invalid2) {
    // Undefined symbol `x`
    AST ast = parse(R"(
        let y = x + 1;
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_FALSE(result.diagnostics.empty());
}

TEST(NameResolverTest, Invalid3) {
    // Undefined symbol `x`
    AST ast = parse(R"(
        {
            let x = 1;
        };
        let y = x;
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_FALSE(result.diagnostics.empty());
}

TEST(NameResolverTest, Invalid4) {
    // Redeclaration of `x`
    AST ast = parse(R"(
        let x = 1;
        let x = 2;
    )");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_FALSE(result.diagnostics.empty());
}

TEST(NameResolverTest, Invalid5) {
    AST ast = parse(R"(
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
    EXPECT_FALSE(result.diagnostics.empty());
}
