#include "Array.hpp"

#include "Dynamic.hpp"

namespace Spark {

    Array::Array() {
        vectorPtr = new std::vector<Dynamic>();
    }

    Array::~Array() {
        delete vectorPtr;
    }

    Dynamic& Array::operator[](Integer index) {
        return (*vectorPtr)[index];
    }

    const Dynamic& Array::operator[](Integer index) const {
        return (*vectorPtr)[index];
    }

} // Spark