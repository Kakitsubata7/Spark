#pragma once

#include <cstddef>

#include "source_buffer.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a reader class for `SourceBuffer`.
 */
class SourceReader {
private:
    SourceBuffer* _srcbufp;
    size_t _index = 0;

public:
    explicit SourceReader(SourceBuffer& srcbuf) noexcept : _srcbufp(&srcbuf) { }

    SourceReader(const SourceReader& other) noexcept = default;
    SourceReader& operator=(const SourceReader& other) noexcept = default;
    SourceReader(SourceReader&& other) noexcept = default;
    SourceReader& operator=(SourceReader&& other) noexcept = default;

    /**
     * Reads a string chunk with a max size of @p maxSize into @p buf. Returns the number of characters read.
     * @param buf Buffer to write into.
     * @param maxSize Max size of the buffer.
     * @return Number of characters read.
     */
    size_t readChunk(char* buf, size_t maxSize) noexcept;
};

} // Spark::FrontEnd
