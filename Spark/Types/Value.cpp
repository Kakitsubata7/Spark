#include "Value.hpp"

#include "../GC/GC.hpp"

namespace Spark {

    Value Value::makeString(Spark::GC& gc, const std::string& value) {
        Value self;
        self.type = Type::String;
        self.stringPtr = gc.make<std::string>(value);
        return self;
    }

    Value Value::makeArray(Spark::GC &gc, const std::vector<Value>& value) {
        Value self;
        self.type = Type::Array;
        self.arrayPtr = gc.make<std::vector<Value>>(value);
        return self;
    }

} // Spark
