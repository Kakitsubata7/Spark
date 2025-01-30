# Type

## Immutability
In type declaration, use the `^` operator to state a type is immutable.

In function argument declaration for example:
```spark
fn foo(a: MyClass^, b: MyClass) -> MyClass^ do
    // Compile-time error because a cannot be modified
    a.mutableMethod()
    
    // No error
    b.mutableMethod()
    
    // Compile-time error because attempting to convert
    // an immutable type to be mutable
    let c: MyClass = a
    
    // This is fine because b is converted to an immutable
    // type
    return b
end
```
From the above example, it's also evident that it's okay to convert from a mutable type to an immutable type, but will result in compile-time error if trying to convert an immutable type to a mutable type.

Note that in the context of *Spark*, immutability is not the same as constness. Immutability is used to describe types, while constness is used to describe variables not being able to reassigned after initialization.


## Type Union
It's possible to declare a type as a **type union**.

When declaring a variable with a type union. For example:
```spark
let u: String | Real = "abc"
u = 1.23
print(typeof(u)) // Real
```
For function arguments and return types:
```spark
fn foo(x: Int | Real) -> String | Int do /* ... */ end
foo(1)              // Fine
const u = foo(3.14) // Fine
foo("string")       // Compile-time error

foo(u)
// Compile-time error since the compiler is unable to
// know if u is String or Int

if u is Int do
    foo(u)
end
// This is fine
// or use the do-if statement
do foo(u) if u is Int
```
This means passing a `Int` or a `Real` is fine for the argument `x`, but not `String` since the type union doesn't include `String`.
