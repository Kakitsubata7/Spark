#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Spark {

    /* ===== Types ===== */

    /* Type IDs */

    enum class Type : uint8_t {
        None = 0x00,
        Integer = 0x01,
        Float = 0x02,
        Boolean = 0x03,
        String = 0x04,
        Array = 0x05,
        Object = 0x06,
        Coroutine = 0x07,
        Thread = 0x08
    };



    /* Integral Types */

    using Integer = int64_t;
    using Float = double;



    /* Boolean Type */

    using Boolean = bool;
    inline constexpr Boolean TRUE = true;
    inline constexpr Boolean FALSE = false;



    /* String Type */

    struct String {
    private:
        std::string* str;
    };



    /* Reference Types */

    struct Dynamic;

    struct Array {
    private:
        std::vector<Dynamic>* vectorPtr;
        constexpr std::vector<Dynamic>& vector() {
            return *vectorPtr;
        }

    public:
        Array();

        ~Array();

        Dynamic& operator[](Integer index);
        const Dynamic& operator[](Integer index) const;
    };

    struct Object {
    private:
        std::unordered_map<std::string, Dynamic>* mapPtr;
        constexpr std::unordered_map<std::string, Dynamic>& map() {
            return *mapPtr;
        }

    public:
        Dynamic& getField(const char* name);
        void setField(const char* name, const Dynamic& value);
    };

    struct Dynamic {
    public:
        union {
            Integer integerValue;
            Float floatValue;
            Boolean booleanValue;
            String stringValue;
            Array arrayValue;
            Object objectValue;
        };
    };



    /* ===== Static Asserts ===== */

    static_assert(sizeof(Integer) == 8, R"(The size of type "Spark::Integer" needs to be exactly 8 bytes.)");
    static_assert(sizeof(Float) == 8, R"(The size of type "Spark::Float" needs to be exactly 8 bytes.)");
    static_assert(sizeof(Dynamic) >= sizeof(void*), R"(The size of type "Spark::Dynamic" cannot be less than the size of a pointer.)");

} // Spark
