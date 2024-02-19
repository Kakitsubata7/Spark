#include <stdio.h>
#include <stdlib.h>

#include "spark/bool.h"

int main(void) {

    spk_bool8_t bool = SPK_TRUE;

    if (bool)
        printf("Hello world!"); /* Comment */

    return EXIT_SUCCESS;
}
