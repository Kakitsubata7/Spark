#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace Spark::FrontEnd {

/**
 * Represents a buffer to store and lookup lines from a compiling file.
 */
class SourceBuffer {
private:
    std::vector<std::string> _lines;

    std::string _buf;
    bool _endedWithCr = false;
    bool _endedWithNewline = false;

public:
    virtual ~SourceBuffer() = default;

    SourceBuffer& operator=(SourceBuffer&& other) = default;

    /**
      * Gets the number of lines in the buffer.
      * @return Number of lines in the buffer.
      */
    [[nodiscard]]
    virtual size_t lineNum() const noexcept {
        return _lines.size();
    }

    /**
     * Gets the line string from the buffer with line number @p lineno.
     * @param lineno Line number.
     * @return Line string at @p lineno.
     */
    [[nodiscard]]
    virtual const std::string& getLine(size_t lineno) const {
        return _lines[lineno - 1];
    }

    /**
     * Appends a chunk of string to the buffer.
     * @param sv String to append.
     */
    virtual void append(std::string_view sv);

    /**
     * Notifies the buffer EOF is reached or stop expecting more contents.
     */
    virtual void flush();

    /**
     * Clears the current content of the source buffer.
     */
    virtual void clear() noexcept {
        _lines.clear();
        _buf.clear();
    }

private:
    /**
     * Appends a line to the buffer.
     * @param line Line to append.
     */
    void appendLine(std::string line) noexcept {
        _lines.push_back(std::move(line));
    }
};

/**
 * Represents a null object for the `SourceBuffer` class.
 */
class NullSourceBuffer final : public SourceBuffer {
private:
    std::string _empty;

public:
    static NullSourceBuffer& instance() noexcept {
        static NullSourceBuffer instance;
        return instance;
    }

    NullSourceBuffer() = default;

    [[nodiscard]]
    size_t lineNum() const noexcept override { return 0; }

    [[nodiscard]]
    const std::string& getLine(size_t lineno) const override { return _empty; }

    void append(std::string_view sv) override { }

    void flush() override { }

    void clear() noexcept override { }
};

} // Spark::FrontEnd
