## Variable
- A variable works like a box for a value. It stores the variable in memory (stack or heap).
- Variables have types. Once the type of a variable is declared, it cannot be changed.
- A variable can be non-reassignable (declared with `let`) or not reassignable (declared with `const`). The non-reassignability of a variable is also known as "constness" in Spark. Note that in Spark, values cannot be reassigned, only variables (the box that holds the value). In other words, constness refers to the non-reassignability of a variable.

### Declaration
#### `let`/`const`
- `let` declares a lexical-scoped reassignable variable.
- `const` declares a lexical-scoped variable that is not reassignable.
- Syntax:
```spark
let name: type = value
let name = value
let^ name = value
```
```spark
const name: type = value
const name = value
const^ name = value
```
- The type of the variable can also be inferred
```spark
// This is the same as: let s: String = "Hello world!"
let s = "Hello world!"
```

## References
- A reference is an alias to a variable. It can be used to extend the lifetime of a variable. For example, returning the reference of a local variable can result in promoting (boxing) of a variable rather than returning a dangling reference.
- References are completely safe to use on any variables.

### Declarations
#### `ref/cref`
- `ref` declares a lexical-scoped reference to a variable that's reassignable.
- `cref` declares a lexical-scoped reference to a variable, but can't reassign through the reference.


## Functions
- All functions are first-class objects.
- Functions capture free variables by reference and will never capture by value.

### Declarations
#### `fn/cfn`
- Both keywords define a definition-scoped function that's not reassignable.
- `fn` declares a function that captures free variables by non-const references (can reassign to captured variables as long as the variables are declared as reassignable).
- `cfn` declares a function that captures free variables by const references (cannot reassign to captured variables).
- `fn^/cfn^`: by adding the `^` operator (immutability requesting operator) after the function definition keyword, it means the function cannot mutate any captured variable's value.

## Error Handling
- Spark does not use exceptions for error-handling for a few reasons:
  - Exceptions usually require stack-unwinding and it's not friendly towards RC and RAII, resulting in a bunch of generated exception landing pad.
  - Traditional exceptions delay error handling of a function rather than encouraging it. Without good exception safety, the codebase can get really messy.
  - Machine code generation complexity.


- Instead, Spark encourages the caller to handle errors and requires explicit error propagation.


### Declaration Syntax
- To allow a function to throw errors, the function needs to be defined with a `throw` keyword before the function scope (before the `do` keyword). Example:
```spark
fn foo() -> String throw do
  // ...
end
```
- Here, `foo` is a function that returns a `String` and can throw an unspecified error. The compiler will automatically infer the throwing type(s).
- In order to specify the throwing type, add the type(s) in after the throw keyword in parentheses. Example:
```spark
fn foo() -> String throw(Error) do
  // ...
end

fn bar() -> Int throw(ArgError | RuntimeError | Array<String>) do
  // ...
end
```
- In the above example, `foo` is a function that explicitly throws an `Error` type; `bar` is a function that explicitly throws one of the three types (`ArgError`, `RuntimeError`, `Array<String>`).
- Note that the syntax for explicit throw types is similar to a type union `(Type1 | Type2 | ...)`. This is because internally, Spark compiles throwing functions to return a union type. Moreover, error handling through the `try/catch` syntax is just a syntax sugar for pattern matching on a type union. For instance, `bar`'s return type can be seen as `(Int | ArgError | RuntimeError | Array<String>)`.

### Handling Syntax
- Spark requires caller of throwing functions to handle the error explicitly with the `try` keyword. To be more specific, the `try/else` statement:
```spark
// `foo` is defined as `fn foo() -> Int throw`
let i = try foo() else 0 // If an error is thrown, use 0 as the default value

// `bar` is defined as `fn bar() -> Void throw`
try bar() else throw // Explicitly propagates the error to the caller
```
- Note that there's can only be an expression after the `else` keyword instead of a statement.
- For more complex error handling, Spark also provides `try/catch` statement:
```spark
// `foo` is defined as `fn foo() -> String throw(ArgError | RuntimeError)`
try do
  print(foo())
end catch e: ArgError do
  // Handle `ArgError`
end catch e: RuntimeError do
  // Handle `RuntimeError`
end catch _ do
  // Handle any other cases
  throw // Use `throw` to propagate the error to the caller
end
```
