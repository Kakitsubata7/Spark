#include "diagnostic.hpp"

namespace Spark {

void Diagnostic::render(std::ostream& os,
                        const std::optional<std::string>& filename) const {
    // Location & message
    if (filename.has_value()) {
        os << filename.value() << ":";
    }
    os << *this << "\n";

    // TODO: Snippet

    // Sub-diagnostics
    for (const Diagnostic& sub : subs) {
        sub.render(os, filename);
        os << "\n";
    }
}

std::ostream& operator<<(std::ostream& os, const Diagnostic& d) {
    os << d.start.line << ":" << d.start.column << ": ";
    switch (d.severity) {
        case Diagnostic::Severity::Note:
            os << "note: ";
            break;
        case Diagnostic::Severity::Warning:
            os << "warning: ";
            break;
        case Diagnostic::Severity::Error:
            os << "error: ";
            break;
        default:
            break;
    }
    os << d.message;
    return os;
}

size_t Diagnostics::count() const noexcept {
    return _diagnostics.size();
}

bool Diagnostics::empty() const noexcept {
    return _diagnostics.empty();
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

void Diagnostics::add(Diagnostic diagnostic) {
    _diagnostics.push_back(std::move(diagnostic));
}

void Diagnostics::adopt(Diagnostics& from) {
    for (Diagnostic& d : from._diagnostics) {
        add(std::move(d));
    }
    from.clear();
}

void Diagnostics::clear() noexcept {
    _diagnostics.clear();
}

void Diagnostics::render(std::ostream& os,
                         const std::optional<std::string>& filename) const {
    for (const Diagnostic& d : _diagnostics) {
        d.render(os, filename);
        os << "\n\n";
    }
}

} // Spark
