#include "source_buffer.hpp"

#include <stdexcept>

namespace Spark::FrontEnd {

std::string_view SourceBuffer::get(Location start, Location end) const {
    std::optional<size_t> s = locToIndex(start);
    std::optional<size_t> e = locToIndex(end);
    if (!s || !e || *e < *s) {
        throw std::out_of_range("invalid source range");
    }
    return std::string_view(_src.data() + *s, (*e - *s) + 1);
}

void SourceBuffer::load(std::istream& stream) {
    // Begin index is the start of the new source
    const size_t beginIndex = _src.size();

    // Append the entire stream
    {
        std::string loaded = std::string(std::istreambuf_iterator<char>(stream),
                                         std::istreambuf_iterator<char>());
        if (!stream.good()) {
            throw std::runtime_error("I/O error while loading from stream");
        }
        _src.append(loaded);
    }

    // If there's any previously loaded lines, removed the last identified line because we need to re-indetify it
    if (!_lines.empty()) {
        _lines.pop_back();
    }

    // Identify lines
    const char* lineBegin = &(_src[beginIndex]);
    size_t lineLen = 0;
    for (size_t i = beginIndex; i < _src.size(); ++i) {
        char c = _src[i];
        if (c == '\n') {
            // LF
            _lines.emplace_back(lineBegin, lineLen);
            lineBegin += lineLen + 1;
            lineLen = 0;
        } else if (c == '\r') {
            // CR/CRLF
            const char* oldLineBegin = lineBegin;
            if (i != _src.size() - 1 && _src[i + 1] == '\n') {
                // CRLF
                ++i; // Skip '\n' in the next iteration
                ++lineBegin; // Pre-skip '\n' for next line
            }
            _lines.emplace_back(oldLineBegin, lineLen);
            lineBegin += lineLen + 1;
            lineLen = 0;
        } else {
            ++lineLen;
        }
    }

    // Append the last line (empty if ended with a newline)
    _lines.emplace_back(lineBegin, lineLen);
}

std::optional<size_t> SourceBuffer::locToIndex(Location loc) const noexcept {
    auto [lineno, columnno] = loc;

    // Invalid lineno
    if (lineno == 0 || lineno > lineNum()) {
        return std::nullopt;
    }

    // Invalid columnno
    std::string_view line = getLine(lineno);
    size_t lineLen = line.size();
    if (columnno == 0 || columnno > lineLen) {
        return std::nullopt;
    }

    return static_cast<size_t>(line.data() - _src.data()) + (columnno - 1);
}

} // Spark::FrontEnd
