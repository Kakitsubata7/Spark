#include "diagnostic.hpp"

namespace Spark {

void Diagnostic::render(std::ostream& os,
                        const std::optional<std::string>& filename) const {
    // Location
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
    os << message << "\n";

    // TODO: Snippet

    // Sub-diagnostics
    for (const Diagnostic& sub : subs) {
        sub.render(os, filename);
        os << "\n";
    }
}

bool Diagnostics::hasWarning() const noexcept {
    for (const Diagnostic& d : _diagnostics) {
        if (d.severity == Diagnostic::Severity::Warning) {
            return true;
        }
    }
    return false;
}

bool Diagnostics::hasError() const noexcept {
    for (const Diagnostic& d : _diagnostics) {
        if (d.severity == Diagnostic::Severity::Error) {
            return true;
        }
    }
    return false;
}

Diagnostic& Diagnostics::add(Diagnostic diagnostic) {
    _diagnostics.push_back(std::move(diagnostic));
    return _diagnostics.back();
}

void Diagnostics::render(std::ostream& os,
                         const std::optional<std::string>& filename) const {
    for (const Diagnostic& d : _diagnostics) {
        d.render(os, filename);
        os << "\n\n";
    }
}

} // Spark
