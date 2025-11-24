#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace Spark::Compiler {

/**
 * Represents a buffer to store and lookup lines from a compiling file.
 */
class SourceBuffer {
private:
    std::vector<std::string> _lines;

public:
    /**
     * Adds a new line to the buffer.
     * @param line Line string to add.
     */
    void addLine(std::string line) {
        _lines.push_back(std::move(line));
    }

    /**
     * Gets the line string from the buffer with line number @p lineno.
     * @param lineno Line number.
     * @return Line string at @p lineno.
     */
    [[nodiscard]]
    const std::string& getLine(size_t lineno) const {
        return _lines[lineno - 1];
    }

    /**
     * Clears the current content of the source buffer.
     */
    void clear() noexcept {
        _lines.clear();
    }
};

} // Spark::Compiler
