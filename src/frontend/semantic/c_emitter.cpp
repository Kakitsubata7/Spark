#include "c_emitter.hpp"

namespace Spark::FrontEnd {

std::string CEmitter::emit() {
    std::string src;

    // Headers
    src += R"(// Headers
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
)";

    // Built-in type declarations
    src += R"(// Built-in type declarations
typedef int spk_Void;

typedef int64_t spk_Int;

typedef double spk_Real;

typedef bool spk_Bool;

typedef struct {
    const char* ptr;
    size_t len;
} spk_String;

typedef int spk_Nil;
)";

    // Type forward declarations

    // Type declarations

    // Runtime function declarations
    src += R"__SPK__(// Runtime functions
spk_String* rt_str(const char* s) {
    spk_String* str = malloc(sizeof(spk_String));
    str->ptr = s;
    str->len = strlen(s);
    return str;
}
)__SPK__";

    // Function forward declarations
    src += R"__SPK__(
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
)__SPK__";

    // Function declarations

    // Variables

    // Main
    src += "int main(int argc, char* argv[]) {";
    src += "    return EXIT_SUCCESS;";
    src += "}";

    return src;
}

std::string CEmitter::intLiteral(const BigInt& i) noexcept {
    return "(int64_t)" + i.str();
}

std::string CEmitter::realLiteral(const BigReal& r) noexcept {
    return "(double)" + r.str();
}

std::string_view CEmitter::boolLiteral(bool b) noexcept {
    return b ? "true" : "false";
}

std::string CEmitter::stringLiteral(std::string_view s) noexcept {
    std::string str;
    str += "rt_str(\"";
    str += s;
    str += "\")";
    return str;
}

std::string_view CEmitter::voidLiteral() noexcept {
    return "0";
}

std::string_view CEmitter::nilLiteral() noexcept {
    return "NULL";
}

} // Spark::FrontEnd
