#pragma once

#include <vector>

#include "Integer.hpp"

namespace Spark {

    class Dynamic;

    class Array {

    private:
        std::vector<Dynamic>* vectorPtr;

    public:
        Array();

        ~Array();

        Dynamic& operator[](Integer index);
        const Dynamic& operator[](Integer index) const;

    };

} // Spark
