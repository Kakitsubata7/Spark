#include "diagnostic.hpp"

#include <sstream>

#include "frontend/semantic/symbol.hpp"

namespace Spark {

using namespace FrontEnd;

Diagnostic Diagnostic::redeclareError(Location start, Location end, const Symbol* sym) {
    std::ostringstream oss;
    oss << "redeclaration of `" << sym->name() << "`";
    return error(start, end, oss.str(), {
        note(sym->start(), sym->end(), "previously declared here")
    });
}

Diagnostic Diagnostic::conflictingRedeclareError(Location start, Location end, const Symbol* sym) {
    std::ostringstream oss;
    oss << "conflicting declarations of `" << sym->name() << "`: previously declared as a ";
    switch (sym->kind) {
        case SymbolKind::Var:
            oss << "variable";
            break;
        case SymbolKind::Func:
            oss << "function";
            break;
        case SymbolKind::Type:
            oss << "type";
            break;
        case SymbolKind::Module:
            oss << "module";
            break;
        default:
            // Unreachable
            oss << "unknown";
            break;
    }
    return error(start, end, oss.str(), {
        note(sym->start(), sym->end(), "previously declared here")
    });
}

Diagnostic Diagnostic::useBeforeDeclError(Location start, Location end, const Symbol* sym) {
    std::ostringstream oss;
    oss << "`" << sym->name() << "` is used before declaration";
    return error(start, end, oss.str(), {
        note(sym->start(), sym->end(), "previously declared here")
    });
}

Diagnostic Diagnostic::cannotFindError(Location start, Location end, std::string_view name) {
    std::ostringstream oss;
    oss << "cannot find symbol `" << name << "`";
    return error(start, end, oss.str());
}

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
