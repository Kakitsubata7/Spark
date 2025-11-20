#pragma once

#include <optional>
#include <string>
#include <vector>

namespace Spark::Compiler::AST {

struct TypeModifiers final {
    bool isImmutable;
    bool isNullable;

    constexpr TypeModifiers(bool isImmutable, bool isNullable) noexcept
        : isImmutable(isImmutable), isNullable(isNullable) { }
};

struct Name {
    std::string name;
    size_t line = 0;
    size_t column = 0;
};

struct Path {
    std::vector<Name> names;
};

struct Pattern;

struct VarDecl {
    enum class DeclType {
        None, Let, Const, Ref, Cref
    };

    DeclType declType = DeclType::None;
    bool isImmutable = false;
    Pattern* pattern = nullptr;
    std::optional<Path> typePath = std::nullopt;

    VarDecl() = default;

    VarDecl(const VarDecl& other) = delete;
    VarDecl& operator=(const VarDecl& other) = delete;

    VarDecl(VarDecl&& other) noexcept : pattern(other.pattern) {
        other.pattern = nullptr;
    }
    VarDecl& operator=(VarDecl&& other) noexcept {
        if (this == &other) {
            pattern = other.pattern;
            other.pattern = nullptr;
        }
        return *this;
    }
};

struct ArgDecl {
    VarDecl var;
    Expr* def = nullptr;
};

enum class AssignType {
    None,
    Assign,
    AddAssign, SubAssign, MulAssign, DivAssign, ModAssign,
    BitAndAssign, BitOrAssign, BitXorAssign, BitShlAssign, BitShrAssign,
    CoalesceAssign
};

} // Spark::Compiler::AST
