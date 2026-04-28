#include "diagnostic.hpp"

#include <algorithm>
#include <sstream>

#include "frontend/semantic/symbol.hpp"

namespace Spark {

using namespace FrontEnd;

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
    // Main
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

    // Subs
    if (!d.subs.empty()) {
        os << "\n";
        for (size_t i = 0; i < d.subs.size(); ++i) {
            os << d.subs[i];
            if (i != d.subs.size() - 1) {
                os << "\n";
            }
        }
    }

    return os;
}

size_t Diagnostics::count() const noexcept {
    return _diagnostics.size();
}

bool Diagnostics::empty() const noexcept {
    return _diagnostics.empty();
}

bool Diagnostics::hasWarning() const noexcept {
    return std::any_of(_diagnostics.begin(), _diagnostics.end(), [](const Diagnostic& d) {
        return d.severity == Diagnostic::Severity::Warning;
    });
}

bool Diagnostics::hasError() const noexcept {
    return std::any_of(_diagnostics.begin(), _diagnostics.end(), [](const Diagnostic& d) {
        return d.severity == Diagnostic::Severity::Error;
    });
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

std::ostream& operator<<(std::ostream& os, const Diagnostics& diagnostics) {
    for (size_t i = 0; i < diagnostics.count(); ++i) {
        os << diagnostics[i];
        if (i != diagnostics.count() - 1) {
            os << "\n";
        }
    }
    return os;
}

void Diagnostics::render(std::ostream& os,
                         const std::optional<std::string>& filename) const {
    for (const Diagnostic& d : _diagnostics) {
        d.render(os, filename);
        os << "\n";
    }
}

} // Spark
