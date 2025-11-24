#include "source_buffer.hpp"

namespace Spark::FrontEnd {

void SourceBuffer::append(std::string_view sv) {
    if (sv.empty()) {
        return;
    }

    // If previously ended with a CR and this chunk starts with LF, don't append a new empty line
    if (_endedWithCr && sv.front() == '\n') {
        _endedWithCr = false;
        sv = sv.substr(1);
    }

    for (size_t i = 0; i < sv.size(); ++i) {
        char c = sv[i];

        // Handles LF
        if (c == '\n') {
            appendLine(_buf);
            _buf.clear();
            continue;
        }
        // Handles CR and CRLF
        if (c == '\r') {
            appendLine(_buf);
            _buf.clear();
            if (i == sv.size() - 1) {
                _endedWithCr = true;
            } else {
                // Handles CRLF
                if (sv[i + 1] == '\n') {
                    ++i;
                }
            }
            continue;
        }

        _buf.push_back(c);
    }

    // If chunk ends with LF or CR, sets the flag so flush can append a newline if needed
    _endedWithNewline = sv.back() == '\n' || sv.back() == '\r';
}

void SourceBuffer::flush() {
    if (!_buf.empty() || _endedWithNewline) {
        _lines.push_back(_buf);
    }
    _buf.clear();
}

} // Spark::FrontEnd
