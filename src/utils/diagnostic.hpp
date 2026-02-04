#pragma once

#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "location.hpp"

namespace Spark {

struct Diagnostic {
    enum class Severity {
        Note, Warning, Error
    };

    Location start;
    Location end;
    Severity severity = Severity::Note;
    std::string message;
    std::vector<Diagnostic> subs;

    Diagnostic() noexcept = default;

    Diagnostic(Location start,
               Location end,
               Severity severity,
               std::string message,
               std::vector<Diagnostic> subs) noexcept
        : start(start), end(end), severity(severity), message(std::move(message)),
          subs(std::move(subs)) { }

    void render(std::ostream& os,
                const std::optional<std::string>& filename = std::nullopt) const;
};

class Diagnostics {
private:
    std::vector<Diagnostic> _diagnostics;

public:
    Diagnostics() noexcept = default;

    const std::vector<Diagnostic>& diagnostics() const noexcept { return _diagnostics; }

    [[nodiscard]]
    bool empty() const noexcept { return _diagnostics.empty(); }

    [[nodiscard]]
    bool hasWarning() const noexcept;

    [[nodiscard]]
    bool hasError() const noexcept;

    void add(Diagnostic diagnostic);

    void merge(Diagnostics& from) noexcept;

    void clear() noexcept { _diagnostics.clear(); }

    void render(std::ostream& os,
                const std::optional<std::string>& filename = std::nullopt) const;
};

} // Spark
