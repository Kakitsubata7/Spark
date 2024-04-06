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



    /* ===== Constructor ===== */

public:
    explicit Object(const Value& _class) {
        setField("__class", _class);
    }



    /* ===== Operations ===== */

public:
    Value& getField(const std::string& name) {
        // If field doesn't exist
        if (map.find(name) == map.end()) {
            std::ostringstream ss;
            ss << "Object has no field '" << name << "'.";
            throw std::runtime_error(ss.str());
        }

        return map[name];
    }

    void setField(const std::string& name, const Value& value) {
        map[name] = value;
    }

    Value& getClass() {
        return getField("__class");
    }

};

} // Spark
