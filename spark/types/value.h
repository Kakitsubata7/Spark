#ifndef SPARK_TYPES_VALUE_H
#define SPARK_TYPES_VALUE_H

#include "bool.h"
#include "float.h"
#include "int.h"
#include "type.h"

typedef struct spk_array_t spk_array_t;

typedef struct spk_array_t {

    spk_type_t type;

    union {
        spk_int_t int_v;
        spk_float_t float_v;
        spk_bool_t bool_v;
        spk_type_t type_v;
        void* ptr_v;
        spk_array_t* array_p;
    };

} spk_value_t;

#endif /* SPARK_TYPES_VALUE_H */
