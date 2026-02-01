#include "diagnostic.hpp"

#include <string_view>

namespace Spark {

void Diagnostic::render(std::ostream& os, const FrontEnd::SourceBuffer& srcbuf) const {
    // Location info
    if (filename.has_value()) {
        os << filename.value() << ":";
    }
    os << start.line << ":" << start.column << ": ";

    // Message
    switch (severity) {
        case Severity::Note:
            os << "note: ";
            break;
        case Severity::Warning:
            os << "warning: ";
            break;
        case Severity::Error:
            os << "error: ";
            break;
        default:
            break;
    }
    os << message << '\n';

    // Snippet
    if (std::string_view snippet; srcbuf.tryGet(start, end, snippet)) {
        os << snippet << '\n';

        // // TODO: Highlight
        // std::string_view line = srcbuf.getLine(start.line);
        // for (size_t i = 1; i < start.column; ++i) {
        //     if (line[i - 1] == '\t') {
        //         os << '\t';
        //     } else {
        //         os << ' ';
        //     }
        // }
        // os << '^';
        // for (size_t i = start.column + 1; i <= end.column; ++i) {
        //     os << '~';
        // }
        // os << '\n';
    }
}

} // Spark
