#include "error.hpp"

#include <sstream>

namespace Spark {

    std::string Error::render(const FrontEnd::SourceBuffer& srcbuf,
                              std::optional<std::string_view> filename) const {
        std::ostringstream oss;

        // File name
        if (filename.has_value()) {
            oss << filename.value() << ':';
        }

        // Location & message
        oss << start.line << ':' << start.column << ": " << message << '\n';

        // Snippet
        for (size_t lineno = start.line; lineno <= end.line; ++lineno) {
            oss << "   " << lineno << " |" << srcbuf.getLine(lineno) << '\n';
        }

        return oss.str();
    }

} // Spark
