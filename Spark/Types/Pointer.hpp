#pragma once

namespace Spark {

struct Pointer {

    /* ===== Data ===== */

private:
    void* value;



    /* ===== Constructors ===== */

public:
    Pointer(void* value = nullptr) : value(value) { }



    /* ===== Operators ===== */

};

}
