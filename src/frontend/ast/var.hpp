#pragma once

#include <optional>

#include "identifier.hpp"

namespace Spark::FrontEnd::AST {

struct VarDecl {
    enum class DeclType {
        None, Let, Const, Ref, Cref
    };

    DeclType declType = DeclType::None;
    bool isImmutable = false;
    Pattern* pattern = nullptr;
    std::optional<Path> typePath;

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
