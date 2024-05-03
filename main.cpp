#include <cstdlib>
#include <iostream>

#include "Spark/Compilation/Lex/Lexer.hpp"
#include "Spark/Compilation/Parse/AST/EvaluateVisitor.hpp"
#include "Spark/Compilation/Parse/AST/Expressions/AddExpression.hpp"
#include "Spark/Compilation/Parse/AST/Expressions/ConstExpression.hpp"
#include "Spark/Compilation/Parse/AST/Statements/ExpressionStatement.hpp"
#include "Spark/Compilation/Parse/Types/Value.hpp"

using namespace Spark;

int main() {

//    std::string code = R"(
//let a=r'Kaishu is a ball';
//)";
//    std::vector<Token> tokens = Lexer::lex(code);
//
//    for (const Token& token : tokens)
//        std::cout << token << std::endl;

    ExpressionStatement statement {
        std::make_unique<AddExpression>(
            std::make_unique<ConstExpression>(Value::makeInt(1)),
            std::make_unique<ConstExpression>(Value::makeInt(2))
        )
    };
    EvaluateVisitor evaluator;

    Value value;
    if (evaluator.visit(statement, value))
        std::cout << value.intValue << std::endl;

    return EXIT_SUCCESS;
}
