#pragma once

#include <sstream>
#include <string>
#include <unordered_map>

#include "Value.hpp"

namespace Spark {

class Namespace {

    /* ===== Data ===== */

private:
    std::unordered_map<std::string, Value> map;



    /* ===== Constructor ===== */

public:
    Namespace() = default;



    /* ===== Operations ===== */

public:
    Value& getField(const std::string& name) {
        // If field doesn't exist
        if (map.find(name) == map.end()) {
            std::ostringstream ss;
            ss << "Namespace has no field '" << name << "'.";
            throw std::runtime_error(ss.str());
        }

        return map[name];
    }

    void setField(const std::string& name, const Value& value) {
        map[name] = value;
    }

};

} // Spark
