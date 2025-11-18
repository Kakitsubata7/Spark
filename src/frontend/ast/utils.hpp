#pragma once

#include <optional>
#include <string>
#include <vector>

namespace Spark::FrontEnd::AST {

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

} // Spark::FrontEnd::AST
