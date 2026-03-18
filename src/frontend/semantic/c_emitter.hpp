#pragma once

#include <string>

#include "utils/bigint.hpp"
#include "utils/bigreal.hpp"

namespace Spark::FrontEnd {

class CEmitter {
public:
    explicit CEmitter() = default;

    std::string emit();

    static std::string intLiteral(const BigInt& i) noexcept;
    static std::string realLiteral(const BigReal& r) noexcept;
    static std::string_view boolLiteral(bool b) noexcept;
    static std::string stringLiteral(std::string_view s) noexcept;
    static std::string_view voidLiteral() noexcept;
    static std::string_view nilLiteral() noexcept;
};

} // Spark::FrontEnd
