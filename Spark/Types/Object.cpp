#include "Object.hpp"

#include "Dynamic.hpp"

namespace Spark {

    /* ===== Constructors ==== */

    Object::Object() : mapPtr(std::make_unique<std::unordered_map<std::string, Dynamic>>()) { }

    Object::Object(std::initializer_list<std::pair<std::string, Dynamic>> list) : Object() {
        std::unordered_map<std::string, Dynamic>& map = *mapPtr;
        for (const std::pair<std::string, Dynamic>& pair : list) {
            map[pair.first] = pair.second;
        }
    }



    /* ===== Metadata ===== */

    std::unordered_map<std::string, Dynamic>& Object::getMap() const {
        return *mapPtr;
    }

} // Spark
