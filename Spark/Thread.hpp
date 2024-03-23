#pragma once

namespace Spark {

class Thread {

    /* ===== Registers ===== */

private:
    void* programCounter;
    void* stackPointer;
    void* basePointer;



    /* ===== Stack ===== */

private:
    void* stackBuffer;

};

} // Spark
