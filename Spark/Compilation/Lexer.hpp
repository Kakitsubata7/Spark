#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace Spark {

namespace Lexer {

    std::vector<std::string> lex(const std::string& str);

}

} // Spark
