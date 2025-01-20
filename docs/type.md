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
It's possible to declare a type as a **type union** (note this is completely different from a union structure that's defined with the `union` keyword).

For example:
```spark

```
