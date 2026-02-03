#include <gtest/gtest.h>

#include "frontend/parser.hpp"
#include "frontend/semantic/name_resolver.hpp"
#include "utils/error.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

static AST parse(std::string_view source) {
    std::istringstream iss{std::string(source)};
    auto [ast, errors] = Parser::parse(iss);
    EXPECT_TRUE(errors.empty()) << "error during parsing source: \n" << source;
    return std::move(ast);
}

TEST(NameResolverTest, Test1) {
    AST ast = parse(R"(
    let x = 1;
    print(x);
)");
    SymbolTable symTable;
    NameResolveResult result = NameResolver::resolve(ast, symTable);
    EXPECT_TRUE(result.diagnostics.empty());
}
