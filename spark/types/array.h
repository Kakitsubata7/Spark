#ifndef SPARK_TYPES_ARRAY_H
#define SPARK_TYPES_ARRAY_H

#include <stddef.h>

typedef struct spk_value_t spk_value_t;

typedef struct spk_array_t {
    spk_value_t* buffer;
    size_t capacity;
    size_t length;
} spk_array_t;

#endif /* SPARK_TYPES_ARRAY_H */
