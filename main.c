#include <stdio.h>
#include <stdlib.h>

#include "spark/types/bool.h"

int main(void) {

    spk_bool_t bool = SPK_TRUE;

    if (bool)
        printf("Hello world!"); /* Comment */

    return EXIT_SUCCESS;
}
