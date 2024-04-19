#include "Object.hpp"

namespace Spark {

    Value& Object::getField(const std::string& name) {
        // If field doesn't exist
        if (map.find(name) == map.end()) {
            std::ostringstream ss;
            ss << "Object has no field '" << name << "'.";
            throw std::runtime_error(ss.str());
        }

        return map[name];
    }

    void Object::setField(const std::string& name, const Value& value) {
        map[name] = value;
    }

} // Spark
