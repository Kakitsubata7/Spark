# Keywords

### `do` `end`
`do` and `end` are a pair of keywords are used to define a lexical scope.
```spark
do
    // ...
end
```

### `fn`
Used to declare a function.
```spark
// Functions without a return type
fn foo() do
    // ...
end
fn foo2() -> Void do
    // ...
end

// Function with a return type
// Argument types need to be explicitly stated
fn bar(n: Real, i: Int) -> String do
    // ...
end
```

### `let`
Used to declare a lexical-scoped variable.
```spark
let foo: Int = 123 // Explicitly typed as Int
let bar = 123 // Type is infered as Int
```

### `const`
Same as `let` which declares a lexical-scoped variable, but the value is immutable.
```spark
const foo = 123
foo = 321 // Compile-time error

const foo = Foo.new()
foo.bar() // This will be fine if bar is an immutable method
```

### `module`
Used to declare a module.

File-scoped module declaration:
```spark
module Foo
// The entire file (or code piece) is under the module Foo
```

Lexical-scoped module declaration:
```spark
module Foo do
    // Code under the module Foo
end

module Bar do
    // Code under the module Bar
end
```

Nested module declaration (works for both file-scoped and lexical-scoped declarations):
```spark
module Foo.Bar do
    // ...
end

// is the same as

module Foo do
    module Bar do
        // ...
    end
end
```
