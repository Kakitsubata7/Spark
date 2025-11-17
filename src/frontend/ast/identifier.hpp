#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace Spark::FrontEnd::AST {

struct Name {
    std::string name;
    size_t line = 0;
    size_t column = 0;
};

struct Path {
    std::vector<Name> names;
};

} // Spark::FrontEnd::AST
