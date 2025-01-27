# Function
In *Spark*, functions are closures, meaning a function always captures values (including other functions). Functions are also objects, so they can be passed around other functions. Besides, this also means a function's lifetime is completely managed by GC. This includes a function's compiled code and captured values. Note that this is the behavior of functions in embedded *Spark* environment. For AOT *Spark* compilation, compiled function code is stored in the executable or library file.

## Generic
Function argument and return types can be *generic*, meaning the type is a set of types similar to type union. However, this set can be infinitely large (by default), but you can limit the inclusion of a generic type.

In *Spark*, generic functions are compiled similar to templates in *C++* where each type (or each combination of types) will produce different machine code.

Here's an example of declaring a generic function in *Spark*:
```spark
fn foo<T>(x: T) -> T do
    // ...
end
```
Here, the type of `x` is unknown to the function `foo<T>` until when the function is invoked with a type parameter:
```spark
foo<Int>(100)
```
The corresponding version of `foo` with `Int` being `T` will be called (emitted as machine code first if not yet so). Note that in *Spark*, as long as the generic function with a specific type is referenced in the code pieces that it is compiled with it, the machine code of that specific function will be emitted. If a reference of the generic function with a different type that is not yet emitted is found in a later loaded code, the *Spark* runtime will emit the machine code of the function with that specific type.

You can also limit the generic type to a subset of types to gain more information of the type. For example, if there's a base class `Animal` with method `makeSound`:
```spark
fn foo<T: Animal>(animal: T) do
    animal.makeSound()
end
```
You can also define function of the same name with different limitation to the generic type as long as they don't intersect. For example, you cannot define `foo<T: Animal>` and `foo<T>` at the same time since the latter also includes `T` when it's `Animal`.

However, you can still define specific implementation of a generic function since they are considered to have higher priority. For example:
```spark
// Generic function declaration
fn calculate<O: Operation>(op: O^) end

// Implementations with specific types
fn calculate<AddOperation>(op: AddOperation^) do
    // ...
end
fn calculate<SubtractOperation>(op: SubtractOperation^) do
    // ...
end
```

Lastly, you can also combine generic functions with pattern matching to handle different concrete types.

## Function Attributes
### `@extern`
Target: any function.

Can be used to indicate the compiler that the function's implementation is provided externally (outside the current code pieces).
```spark
@extern
fn printf(format: String, ...: String) end

printf("Hello %s!", "World") // Prints "Hello World!" to stdout
```

### `@inline`
Target: any function (except extern functions).

Can be used to indicate the compiler to inline the function aggressively.

### `@static`
Target: methods (and other kinds of fields) of class, struct, union, or enum.

Can be used to indicate the compiler that a field of a class, a struct, a union, or an enum is *static*, meaning the function is not associated with the instance, but rather the class itself. 

### `@virtual`
Target: methods of class.

Can be used to indicate the compiler that the method of a class is *virtual*, meaning it can be overridden by subclasses if they want.

### `@override`
Target: methods of class.

Can be used to indicate the compiler that the method is overriding a base class method of the same name (and types).

If the base method is not virtual (declared with `@virtual`), a compile-time error will be raised. Similarly, if defining a method that has the same name (and types) as a base virtual method without `@override`, a compile-time error will be raised.
