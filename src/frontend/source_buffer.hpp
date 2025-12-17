#pragma once

#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "utils/location.hpp"

namespace Spark::FrontEnd {

/**
 * Represents a buffer to store and lookup lines from a source file.
 */
class SourceBuffer {
private:
    std::string _src;
    std::vector<std::string_view> _lines;

protected:
    SourceBuffer(std::string src, std::vector<std::string_view> lines) noexcept
        : _src(std::move(src)), _lines(std::move(lines)) { }

public:
    explicit SourceBuffer(std::istream& stream) {
        SourceBuffer::load(stream);
    }
    explicit SourceBuffer(const std::string& s) {
        std::istringstream iss(s);
        SourceBuffer::load(iss);
    }
    SourceBuffer() = default;

    virtual ~SourceBuffer() = default;

    SourceBuffer(const SourceBuffer& other) = default;
    SourceBuffer& operator=(const SourceBuffer& other) = default;
    SourceBuffer(SourceBuffer&& other) noexcept = default;
    SourceBuffer& operator=(SourceBuffer&& other) noexcept = default;

    /**
     * Number of lines of the source.
     * @return Number of lines of the source.
     */
    [[nodiscard]]
    size_t lineNum() const noexcept {
        return _lines.size();
    }

    /**
     * Clears the buffer.
     */
    void clear() noexcept {
        _lines.clear();
    }

    /**
     * Retrieves a substring view from @p start to @p end (inclusive).
     * @param start Start location of the substring view (inclusive).
     * @param end End location of the substring view (inclusive).
     * @return Retrieved substring view.
     */
    [[nodiscard]]
    virtual std::string_view get(Location start, Location end) const;

    /**
     * Retrieves the line (without a trailing newline character) at @p lineno.
     * @param lineno Number of the line to retrieve.
     * @return Retrieved line.
     */
    [[nodiscard]]
    virtual std::string_view getLine(size_t lineno) const {
        return _lines.at(lineno - 1);
    }

    /**
     * Loads the entire stream's content into the source buffer.
     * @param stream Stream to load from.
     */
    virtual void load(std::istream& stream);

private:
    /**
     * Converts a location in the source lines to index in the flat source string.
     * @param loc Location in the source lines.
     * @return Index in the flat source string.
     */
    [[nodiscard]]
    std::optional<size_t> locToIndex(Location loc) const noexcept;

    friend class SourceReader;
};

/**
 * Represents a null object for `SourceBuffer` to be used as an empty source buffer.
 */
class NullSourceBuffer final : public SourceBuffer {
private:
    NullSourceBuffer() : SourceBuffer("", {}) { }

public:
    static NullSourceBuffer& instance() noexcept {
        static NullSourceBuffer inst;
        return inst;
    }

    [[nodiscard]]
    std::string_view getLine(size_t lineno) const override {
        return "";
    }

    [[nodiscard]]
    std::string_view get(Location start, Location end) const override {
        return "";
    }

    void load(std::istream& stream) override { }
};

} // Spark::FrontEnd
