#ifndef SPARK_THREAD_H
#define SPARK_THREAD_H

#include <stddef.h>

#include "types/value.h"

typedef struct spk_thread {

    /* ===== Stack ===== */

    spk_value_t* stack_buffer;
    size_t stack_capacity;
    size_t max_stack_capacity;
    size_t stack_length;



    /* ===== Registers ===== */

    void* program_counter;
    spk_value_t* stack_pointer;
    spk_value_t* base_pointer;

} spk_thread;

#endif /* SPARK_THREAD_H */
