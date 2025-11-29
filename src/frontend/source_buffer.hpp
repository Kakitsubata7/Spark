#pragma once

#include <istream>
#include <string>
#include <vector>

namespace Spark::FrontEnd {

/**
 * Represents a buffer to store and lookup lines from a compiling file.
 */
class SourceBuffer {
private:
    std::vector<std::string> _lines;

    mutable size_t _lineIndex = 0;
    mutable size_t _charIndex = 0;

public:
    explicit SourceBuffer(std::istream& stream, size_t chunkSize = 8192) {
        load(stream, chunkSize);
    }
    SourceBuffer() = default;

    /**
     * Number of lines of the source.
     * @return Number of lines of the source.
     */
    [[nodiscard]]
    size_t lineNum() const noexcept {
        return _lines.size();
    }

    /**
     * Clears the buffer.
     */
    void clear() noexcept {
        _lines.clear();
        _lineIndex = 0;
        _charIndex = 0;
    }

    /**
     * Reads source into @p buf which has max size of @p maxSize.
     * @param buf Buffer to write the read content into.
     * @param maxSize Max size of the buffer.
     * @return Number of characters read.
     */
    size_t readChunk(char* buf, size_t maxSize) const noexcept;

    /**
     * Retrieves the line (without a trailing newline character) at @p lineno.
     * @param lineno Number of the line to retrieve.
     * @return Retrieved line.
     */
    const std::string& getLine(size_t lineno) const {
       return _lines.at(lineno - 1);
    }

    /**
     * Loads the entire stream's content by chunks into the source buffer.
     * @param stream Stream to load from.
     * @param chunkSize Size of the chunk.
     */
    void load(std::istream& stream, size_t chunkSize);
};

} // Spark::FrontEnd
