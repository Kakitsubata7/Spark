#pragma once

#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "location.hpp"

namespace Spark {

namespace FrontEnd {
    struct Symbol;
} // FrontEnd

/**
 * Represents a compilation diagnostic with source location, severity, message, and sub-diagnostics.
 */
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

    Diagnostic(const Diagnostic& other) noexcept = default;
    Diagnostic& operator=(const Diagnostic& other) noexcept = default;

    Diagnostic(Diagnostic&& other) noexcept = default;
    Diagnostic& operator=(Diagnostic&& other) noexcept = default;

    /**
     * Constructs a note diagnostic.
     * @param start Start location in source.
     * @param end End location in source.
     * @param message Diagnostic message.
     * @param subs Sub-diagnostics.
     * @return Constructed note diagnostic.
     */
    static Diagnostic note(Location start,
                           Location end,
                           std::string message,
                           std::vector<Diagnostic> subs = {}) noexcept {
        return {start, end, Severity::Note, std::move(message), std::move(subs)};
    }

    /**
     * Constructs a warning diagnostic.
     * @param start Start location in source.
     * @param end End location in source.
     * @param message Diagnostic message.
     * @param subs Sub-diagnostics.
     * @return Constructed warning diagnostic.
     */
    static Diagnostic warning(Location start,
                              Location end,
                              std::string message,
                              std::vector<Diagnostic> subs = {}) noexcept {
        return {start, end, Severity::Warning, std::move(message), std::move(subs)};
    }

    /**
     * Constructs an error diagnostic.
     * @param start Start location in source.
     * @param end End location in source.
     * @param message Diagnostic message.
     * @param subs Sub-diagnostics.
     * @return Constructed error diagnostic.
     */
    static Diagnostic error(Location start,
                            Location end,
                            std::string message,
                            std::vector<Diagnostic> subs = {}) noexcept {
        return {start, end, Severity::Error, std::move(message), std::move(subs)};
    }

    /**
     * Constructs an error for name resolution redeclaration error.
     * @param start Start location in source.
     * @param end End location in source.
     * @param sym Symbol that causes the redeclaration.
     * @return Constructed error diagnostic.
     */
    static Diagnostic redeclareError(Location start, Location end, const FrontEnd::Symbol* sym);

    /**
     * Constructs an error for name resolution cannot find error.
     * @param start Start location in source.
     * @param end End location in source.
     * @param name Name that couldn't be found.
     * @return Constructed error diagnostic.
     */
    static Diagnostic cannotFindError(Location start, Location end, std::string_view name);

    /**
     * Renders the diagnostic into the output stream.
     * @param os Output stream.
     * @param filename File name (optional).
     */
    void render(std::ostream& os,
                const std::optional<std::string>& filename = std::nullopt) const;

    friend std::ostream& operator<<(std::ostream& os, const Diagnostic& d);
};

/**
 * Represents a collection of diagnostics.
 */
class Diagnostics {
private:
    std::vector<Diagnostic> _diagnostics;

public:
    Diagnostics() noexcept = default;

    [[nodiscard]]
    const std::vector<Diagnostic>& diagnostics() const noexcept { return _diagnostics; }

    /**
     * Gets the number of diagnostics.
     * @return Number of diagnostics.
     */
    [[nodiscard]]
    size_t count() const noexcept;

    /**
     * Checks if there's any diagnostics.
     * @return true if there's no diagnostics, false otherwise.
     */
    [[nodiscard]]
    bool empty() const noexcept;

    /**
     * Checks if there's at least one warning diagnostic.
     * @return true if there's at least one warning diagnostic, false otherwise.
     */
    [[nodiscard]]
    bool hasWarning() const noexcept;

    /**
     * Checks if there's at least one error diagnostic.
     * @return true if there's at least one error diagnostic, false otherwise.
     */
    [[nodiscard]]
    bool hasError() const noexcept;

    /**
     * Adds a new diagnostic.
     * @param diagnostic Diagnostic to add.
     */
    void add(Diagnostic diagnostic);

    /**
     * Takes all diagnostics and move it to self.
     * @param from Diagnostics to be taken from.
     */
    void adopt(Diagnostics& from);

    /**
     * Removes all diagnostics.
     */
    void clear() noexcept;

    /**
     * Renders all diagnostics into the output stream.
     * The order of diagnostics is based on the adding order.
     * @param os Output stream.
     * @param filename File name (optional).
     */
    void render(std::ostream& os,
                const std::optional<std::string>& filename = std::nullopt) const;
};

} // Spark
