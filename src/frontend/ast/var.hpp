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
};

} // Spark::FrontEnd::AST
