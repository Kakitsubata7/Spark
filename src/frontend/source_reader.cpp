#include "source_reader.hpp"

#include <algorithm>
#include <cstring>
#include <string>

namespace Spark::FrontEnd {

size_t SourceReader::readChunk(char* buf, size_t maxSize) noexcept {
    const std::string& src = _srcbuf._src;
    if (_index >= src.size()) {
        return 0;
    }

    size_t remaining = src.size() - _index;
    size_t toRead = std::min(remaining, maxSize);

    std::memcpy(buf, src.data() + _index, toRead);
    _index += toRead;

    return toRead;
}

} // Spark::FrontEnd
