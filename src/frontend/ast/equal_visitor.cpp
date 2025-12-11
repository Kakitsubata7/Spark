#include "equal_visitor.hpp"

#include <typeinfo>

#include "node.hpp"

namespace Spark::FrontEnd {

static bool typeidEquals(const Node& lhs, const Node& rhs) {
    return typeid(lhs) == typeid(rhs);
}

void EqualVisitor::visit(Node& lhs) {
    if ((_result = typeidEquals(lhs, _rhs))) {
        _result = lhs.start == _rhs.start && lhs.end == _rhs.end;
    }
}

} // Spark::FrontEnd
