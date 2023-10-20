#pragma once

#include <cstdint>
#include <initializer_list>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

namespace Spark {

    class Dynamic;

    class Object {

        /* ===== Constructors ===== */

    public:
        Object();
        Object(std::initializer_list<std::pair<std::string, Dynamic>> list);



        /* ===== Metadata ===== */

    private:
        std::unique_ptr<std::unordered_map<std::string, Dynamic>> mapPtr;

    public:
        [[nodiscard]]
        std::unordered_map<std::string, Dynamic>& getMap() const;



        /* ===== Conversion ===== */

    public:
        [[nodiscard]]
        std::string toString() {
            std::ostringstream oss;
            oss << "<object: 0x"
                << std::uppercase
                << std::setfill('0')
                << std::setw(sizeof(void*) * 2)
                << std::hex
                << reinterpret_cast<uintptr_t>(mapPtr.get())
                << ">";
            return oss.str();
        }

    };

} // Spark
