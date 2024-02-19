#ifndef SPARK_VALUE_H
#define SPARK_VALUE_H

#include "bool.h"
#include "float.h"
#include "int.h"
#include "type.h"

typedef struct {

    spk_type_t type;

    union {
        spk_int_t int_v;
        spk_float_t float_v;
        spk_bool_t bool_v;
        spk_type_t type_v;
    };

} spk_value_t;

#endif /* SPARK_VALUE_H */
