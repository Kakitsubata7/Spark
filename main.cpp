#include <iostream>

#include "Spark/GC/GC.hpp"
#include "Spark/GC/GCPtr.hpp"
#include "Spark/Bool8.hpp"
#include "Spark/Float64.hpp"
#include "Spark/Int64.hpp"
#include "Spark/Type.hpp"
#include "Spark/Value.hpp"
#include "Spark/Thread.hpp"
#include "Spark/Compilation/Lexer.hpp"

using namespace Spark;

int main() {

//    GC gc;
//
//    GCPtr<Int64> p = gc.make<Int64>(3);
//
//    std::cout << "Value: " << *p << std::endl;
//    std::cout << "Ref count: " << p.referenceCount() << std::endl;

//    Value v = Value::makeInteger(1);
//    v = 2;
//    std::cout << v << std::endl;

//    Thread th;
//    th.push(Value::makeNil());
//    th.push(Value::makeInteger(1));
//    th.push(Value::makeFloat(3.25f));
//    th.push(Value::makeBoolean(true));
//    th.push(Value::makeType(Type::Box));
//    th.push(Value::makePointer(nullptr));
//    std::cout << th.stackToString() << std::endl;

    Lexer lexer;
    lexer.append("abc def ghi");
    std::vector<std::string> tokens = lexer.getTokens();
    for (const std::string& token : tokens)
        std::cout << token << std::endl;

    return 0;
}
