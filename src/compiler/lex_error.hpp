#pragma once

#include <ostream>
#include <string>
#include <string_view>

namespace spark {

struct LexError {
    std::string message;
    size_t line;
    size_t column;

    LexError(std::string_view message, size_t line, size_t column)
        : message(message), line(line), column(column) { }

    friend std::ostream& operator<<(std::ostream& os, const LexError& error) {
        return os << error.line << ':' << error.column << ": " << error.message;
    }
};

} // spark
