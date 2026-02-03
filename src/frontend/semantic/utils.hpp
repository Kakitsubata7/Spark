#pragma once

#include "frontend/ast/node.hpp"

namespace Spark::FrontEnd {

[[nodiscard]]
inline bool isHoisted(const Node* node) {
    return node->is<FnDefStmt>() || node->is<TypeDefStmt>() || node->is<ModuleStmt>();
}

} // Spark::FrontEnd
