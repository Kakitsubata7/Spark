#ifndef SPARK_TYPE_H
#define SPARK_TYPE_H

#include <stdint.h>

typedef uint8_t spk_type_t;

#define SPK_TYPE_NIL    0x00
#define SPK_TYPE_INT    0x01
#define SPK_TYPE_FLOAT  0x02
#define SPK_TYPE_BOOL   0x03
#define SPK_TYPE_TYPE   0x04
#define SPK_TYPE_PTR    0x05
#define SPK_TYPE_ARRAY  0x06
#define SPK_TYPE_SET    0x07
#define SPK_TYPE_MAP    0x08
#define SPK_TYPE_OBJECT 0x09

#endif /* SPARK_TYPE_H */
