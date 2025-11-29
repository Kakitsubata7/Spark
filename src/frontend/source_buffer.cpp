#include "source_buffer.hpp"

#include <cstring>
#include <vector>

namespace Spark::FrontEnd {

size_t SourceBuffer::readChunk(char* buf, size_t maxSize) const noexcept {
    size_t read = 0;

    while (read < maxSize && _lineIndex < _lines.size()) {
        std::string_view line = _lines[_lineIndex];
        std::string_view content = line.substr(_charIndex);

        // Reads as much as the buffer can
        size_t toRead = std::min(content.size(), maxSize - read);
        if (toRead > 0) {
            std::memcpy(buf + read, content.data(), toRead);
            read += toRead;
            _charIndex += toRead;

            if (read == maxSize) {
                break;
            }
        }

        // Appends newline in the end when a line is consumed
        if (_charIndex == line.size()) {
            bool hadNewline = _lineIndex + 1 < _lines.size();
            if (hadNewline && read < maxSize) {
                buf[read++] = '\n';
            }
            _lineIndex++;
            _charIndex = 0;
        }
    }

    return read;
}

void SourceBuffer::load(std::istream& stream, size_t chunkSize) {
    std::vector<char> chunk(chunkSize);
    std::string line;
    line.reserve(128);

    while (true) {
        stream.read(chunk.data(), static_cast<std::streamsize>(chunk.size()));
        size_t read = static_cast<size_t>(stream.gcount());

        bool endedWithCR = false;
        for (size_t i = 0; i < read; ++i) {
            char c = chunk[i];
            if (c == '\n') {
                // LF
                _lines.push_back(line);
                line.clear();
                endedWithCR = false;
            } else if (c == '\r') {
                // CR/CRLF
                if (i != read - 1 && chunk[i + 1] == '\n') {
                    // CRLF
                    ++i;
                    endedWithCR = false;
                } else {
                    // CR
                    endedWithCR = true;
                }
                _lines.push_back(line);
                line.clear();
            } else {
                line.push_back(c);
                endedWithCR = false;
            }
        }

        // Handle chunks split by CRLF
        if (endedWithCR) {
            int next = stream.get();
            if (next != '\n' && next != EOF) {
                stream.unget();
            }
        }

        if (stream.eof()) {
            _lines.push_back(line);
            break;
        }

        if (!stream.good()) {
            throw std::runtime_error("I/O error while reading from stream");
        }
    }
}

} // Spark::FrontEnd
