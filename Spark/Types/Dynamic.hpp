#pragma once

#include "Array.hpp"
#include "Boolean.hpp"
#include "Coroutine.hpp"
#include "Float.hpp"
#include "Function.hpp"
#include "Integer.hpp"
#include "Object.hpp"
#include "String.hpp"
#include "Table.hpp"
#include "Thread.hpp"
#include "Type.hpp"

namespace Spark {

    class Dynamic {

        /* ===== Fields ===== */

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
            Function functionValue;
            Coroutine coroutineValue;
            Thread threadValue;
            void* pointerValue;
        };



        /* ===== Constructors ===== */

    public:
        Dynamic() : type(Type::None) {
            integerValue = {};
            pointerValue = {};
        }

        explicit Dynamic(Type type) : type(type) {
            typeValue = {};
            integerValue = {};
            floatValue = {};
            booleanValue = {};
            stringValue = {};
            arrayValue = {};
            tableValue = {};
            objectValue = {};
            functionValue = {};
            coroutineValue = {};
            threadValue = {};
        }

        Dynamic(Type type, Type value) : Dynamic(type) {
            typeValue = value;
        }

        Dynamic(Type type, Integer value) : Dynamic(type) {
            integerValue = value;
        }

        Dynamic(Type type, Float value) : Dynamic(type) {
            floatValue = value;
        }

        Dynamic(Type type, Boolean value) : Dynamic(type) {
            booleanValue = value;
        }



        /* ===== Destructor ===== */

    public:
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



        /* ===== Copying ===== */

    public:
        Dynamic(const Dynamic& other) : type(other.type) {
            integerValue = other.integerValue;
            pointerValue = other.pointerValue;
        }

        Dynamic& operator=(const Dynamic& other) {

            // Self-assignment protection
            if (this == &other)
                return *this;

            // Shallow copy values
            type = other.type;
            integerValue = other.integerValue;
            pointerValue = other.pointerValue;

            return *this;
        }

    };



    /* ===== Static Assert ===== */

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
