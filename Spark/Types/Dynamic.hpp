#pragma once

#include "Array.hpp"
#include "Boolean.hpp"
#include "Float.hpp"
#include "Integer.hpp"
#include "Object.hpp"
#include "String.hpp"
#include "Table.hpp"
#include "Type.hpp"

namespace Spark {

    class Dynamic {

    public:
        Type type;
        union {
            Type typeValue;
            Integer integerValue;
            Float floatValue;
            Boolean booleanValue;
            String stringValue;
            Array arrayValue;
            Table tableValue;
            Object objectValue;
            void* pointerValue;
        };

        Dynamic() {
            type = Type::None;
            integerValue = 0;
            pointerValue = nullptr;
        }

        ~Dynamic() {
            switch (type) {
                case Type::String:
                    stringValue.~String();
                    break;

                case Type::Array:
                    arrayValue.~Array();
                    break;

                case Type::Table:
                    tableValue.~Table();
                    break;

                case Type::Object:
                    objectValue.~Object();
                    break;

                default:
                    break;
            }
        }

    };

    namespace {
        struct SystemSize {
            [[maybe_unused]] Type type;
            [[maybe_unused]] void* pointer;
        };
        static_assert(sizeof(Dynamic) >= sizeof(SystemSize), R"(The size of type "Spark::Dynamic" should be greater )"
                                                             R"(than or equal to the size of a struct with "Spark::Type" )"
                                                             "and a pointer.");
    }

} // Spark
