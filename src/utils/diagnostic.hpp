#pragma once

#include <optional>
#include <ostream>
#include <string>

#include "frontend/source_buffer.hpp"
#include "location.hpp"

namespace Spark {

class Diagnostic {
public:
    enum class Severity {
        Note, Warning, Error, Fatal
    };

    std::optional<std::string> filename;
    Location start;
    Location end;
    Severity severity = Severity::Note;
    std::string message;

    Diagnostic() = default;
    Diagnostic(std::optional<std::string> filename,
               Location start,
               Location end,
               Severity severity,
               std::string message) noexcept
        : filename(std::move(filename)), start(start), end(end), severity(severity), message(std::move(message)) { }

    static Diagnostic note(std::optional<std::string> filename,
                           Location start,
                           Location end,
                           std::string message) noexcept {
        return Diagnostic{std::move(filename), start, end, Severity::Note, std::move(message)};
    }

    static Diagnostic warning(std::optional<std::string> filename,
                              Location start,
                              Location end,
                              std::string message) noexcept {
        return Diagnostic{std::move(filename), start, end, Severity::Warning, std::move(message)};
    }

    static Diagnostic error(std::optional<std::string> filename,
                            Location start,
                            Location end,
                            std::string message) noexcept {
        return Diagnostic{std::move(filename), start, end, Severity::Error, std::move(message)};
    }

    void render(std::ostream& os, const FrontEnd::SourceBuffer& srcbuf) const;
};

} // Spark
