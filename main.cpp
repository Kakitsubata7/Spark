#include <cstdlib>
#include <iostream>
#include <string>

#include "Spark/Compilation/Lex/Lexer.hpp"

using namespace Spark;

int main() {

    std::string code = R"(
/*let a=123.;
let b=.123;
let c=0.123;
let d=-.123;
let e=+123;
let f=+.123;
let g=-12.3;
let h=+123.;*/
let a=-.;
)";
    std::vector<Token> tokens = Lexer::lex(code);

    for (const Token& token : tokens)
        std::cout << token << std::endl;

    return EXIT_SUCCESS;
}
