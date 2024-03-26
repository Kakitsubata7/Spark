#include "Value.hpp"

#include "../GC/GC.hpp"

namespace Spark {

    Value Value::makeString(Spark::GC& gc, const std::string& value) {
        Value self;
        self.type = Type::String;
        self.nodePtr = gc.allocate<std::string>(value);
        return self;
    }

    Value Value::makeArray(Spark::GC& gc, const std::vector<Value>& value) {
        Value self;
        self.type = Type::Array;
        self.nodePtr = gc.allocate<std::vector<Value>>(value);
        return self;
    }

} // Spark
