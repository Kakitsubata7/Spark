#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int spk_Void;

typedef int64_t spk_Int;

typedef double spk_Real;

typedef bool spk_Bool;

typedef struct {
    const char* ptr;
    size_t len;
} spk_String;

typedef int spk_Nil;

spk_String* rt_string(const char* s) {
    spk_String* str = malloc(sizeof(spk_String));
    str->ptr = s;
    str->len = strlen(s);
    return str;
}

void spk_print_spk_Void_spk_Void(spk_Void _) {
    printf("()");
}

void spk_print_spk_Int_spk_Void(spk_Int i) {
    printf("%ld", i);
}

void spk_print_spk_Real_spk_Void(spk_Real r) {
    printf("%f", r);
}

void spk_print_spk_Bool_spk_Void(spk_Bool b) {
    printf(b ? "true" : "false");
}

void spk_print_spk_String_spk_Void(spk_String* str) {
    printf("%s", str->ptr);
}

void spk_print_spk_Nil_spk_Void(spk_Nil _) {
    printf("nil");
}

int main(int argc, char* argv[]) {
    return EXIT_SUCCESS;
}
