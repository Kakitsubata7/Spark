#include <gtest/gtest.h>

#include "frontend/ast.hpp"
#include "frontend/parser.hpp"

using namespace Spark;
using namespace Spark::FrontEnd;

// Parser parse(std::string_view source) {
//     std::istringstream iss{std::string(source)};
//     Parser parser;
//     return parser.parse(iss);
// }
//
// TEST(ParserTest, GeneralTest1) {
//     std::string_view source = R"(while true do end)";
//     auto result = parse(source);
//     EXPECT_TRUE(result.hasValue()) << result.error().message;
// }
