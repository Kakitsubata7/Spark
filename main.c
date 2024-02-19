#include <stdio.h>
#include <stdlib.h>

#include "spark/spk_bool8.h"

int main(void) {

    spk_bool8 bool = SPK_TRUE;

    if (bool)
        printf("Hello world!"); /* Comment */

    return EXIT_SUCCESS;
}
