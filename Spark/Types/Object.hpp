#pragma once

#include <sstream>
#include <string>
#include <unordered_map>

#include "Namespace.hpp"
#include "Value.hpp"

namespace Spark {

class Object {

    /* ===== Data ===== */

private:
    std::unordered_map<std::string, Value> map;



    /* ===== Operations ===== */

public:
    Value& getField(const std::string& name);

    void setField(const std::string& name, const Value& value);

};

} // Spark
