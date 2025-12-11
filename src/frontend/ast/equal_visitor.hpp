#pragma once

#include <typeinfo>

#include "node.hpp"
#include "node_visitor.hpp"

namespace Spark::FrontEnd {

class EqualVisitor final : public NodeVisitor {
private:
    bool _result;
    const Node& _rhs;

public:
    [[nodiscard]]
    constexpr bool result() const noexcept { return _result; }

    explicit EqualVisitor(const Node& rhs) noexcept : _result(false), _rhs(rhs) { }

    void visit(Node& lhs) override;
};

} // Spark::FrontEnd
