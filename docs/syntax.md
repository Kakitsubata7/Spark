## Syntax

### Variable Declaration
#### `let`
- Defines a **reassignable** lexical-scoped variable in the current scope. The name must not already been defined (in the same compilation context).
```
let x = 1
let s: String = "Hello world!"
```
- Explicit type annotations are optional if the type can be inferred.
- There is a syntax sugar for declaring variables of immutable types without explicit type annotations:
```
let^ foo = Foo()
// The above is exactly the same as:
let foo: Foo^ = Foo()
```

- Using `let^` with a explicit mutable type is a compile-time error:
```
let^ foo: Foo = Foo()
// Compile-time error since `let^` expects an immutable type
```

- Assuming `mutate` is a mutating method of type `Foo`, the following code will result in a compile-time error:
```
foo.mutate()
```

#### `const`
- Does what `let` does but the defined variable is not reassignable.
```
const name: String = "Alex"
name = "Bob" // Compile-time error

const foo = Foo()
foo.mutate() // Ok, since `foo` is mutable but not reassignable

const^ bar = Bar()
bar.mutate() // Compile-time error,
             // assuming `mutate` is a mutating method
```

#### Undefining
- A previously existing name in the language runtime can be undefined with either `let` or `const` keyword like so:
```
let foo = undefined
const^ bar: Bar^ = undefined
// Explicit type and mutability don't matter here
```
- If the name doesn't exist, the statement simply does nothing.
- If the value under the name is an overloaded function, all overloadings will be unbound from the name (since the language sees an overloaded function as one value).


### Function Declaration
```
fn talk(msg: String) do
    print(msg)
end

fn add(a: Real, b: Real) -> Real do
    return a + b
end
```
- Functions are **hoisted**, so they can be used before their declaration.

#### First-Class Functions
- Functions are **values** and can be assigned to variables via function expressions.
```
let fun: fn (Int, Int) -> Int = 
    fn (a: Int, b: Int) -> Int do return x + y end
print(fun(2, 1)) // Prints 3
fun = subtract
print(fun(2, 1)) // Prints 1
    
fn subtract(a: Int, b: Int) -> Int do
    return a - b
end
```

#### Parameter Reassignability
- By default, function parameters are not reassignable, but it's possible to explicitly control their reassignability by adding `let` or `const` before the parameter name:
```
fn fun(let arr: Array<Int>^) do
    let temp = arr.clone()
    temp.push(0)
    arr = temp
    print(arr) // Prints { ..., 0 }
end
```

#### Capture Semantics
- Functions always capture variables by **references**. References act like aliases of the original variables, so the lifetime of captured variables may be extended (boxing).
- Functions can also capture by **value** explicitly by specifying them in a capture-by-value list. Captured-by-value variables act like additional parameters without affecting the function's signature.
```
let msg = "Hi"

fn foo()[msg] do
    print(msg)
end

msg = "Bye"

foo() // Prints "Hi" since `msg` is captured by value 
      // instead of reference
```
- Just like function parameters, captured values can be annotated with `let` or `const` (`let^` or `const^` as well) to control whether they are reassignable (and mutable) within the function body.

#### Immutable Functions
- Functions can be declared as **immutable** by adding ^ after fn. An immutable function **cannot** **mutate** or **reassign** any of its captured variables, whether they are captured-by-reference or captured-by-value.
```
let arr: Array<Int> = { 1, 2, 3 }

fn^ foo() do
    arr.push(4)
    // Compile-time error since `push` is mutating
    
    arr = { 1, 2, 3, 4 }
    // Compile-time error since `foo` is an immutable function
end

fn^ bar()[arr] do
    arr.push(4)
    // Compile-time error since `push` is mutating
end
```
- Immutable functions are **not** pure functions. The immutability only applies to captured variables, function parameters are not affected.

#### Outer-scope Name Accessing Operator (`$`)
- The `$` operator is used to explicitly reference variables from an **outer scope** in case the current scope introduces a name collision.
```
let foo = Foo()

fn fun(foo: Foo) do
    $foo.bar() // Referencing the outer `foo` 
               // instead of the parameter
end
```
- The `$` operator can be stacked to access variables from farther enclosing scopes. Each `$` climbs one level outward.
```
let x = 1
const fun = fn () do
    let x = 2
    fn () do
        let x = 3
        fn () do
            print($$$x + $$x + $x) // Prints 6
        end ()
    end ()
end
```

#### Pure Functions
- Pure functions can be enforced through compile-time annotation `@pure`.
- Pure functions can only call other pure functions and any kind of mutation is **not** allowed inside the function body.


### Type Declaration
- Like functions, types are also **hoisted**, meaning they can be referenced before their declaration.

#### Methods
- Types can define methods, which are functions attached to the type and implicitly capture the instance. Methods always have access to `self`, which refers to the current instance.
- Types like class and struct can define **instance fields**, which affect the layout of the type. These fields are treated as **captured** by their methods **syntactically**, similar to captured variables in functions.
- Because of that, the `$` operator can be used to access instance fields. `self` is still accessible in case returning the instance itself is needed. However, modifying instances fields through the `self` keyword is seen as mutating the `self` instance while modifying through the `$` keyword is seen as reassigning a captured variable **syntactically**.
```
class Person do
    @private
    let name: String
    
    fn^ getName() -> String do
        return name
    end
    
    fn setName(name: String) do
        $name = name
    end
end
```

#### Member Accessibility
- Type members (fields and methods) can have different accessibilities, controlled through compile-time annotations: `@public`, `@protected`, `@private`, etc.
- By default, all members are **public** unless an access modifier is explicitly specified.

#### Class
- Classes are **reference** types lives on the heap managed by **Reference Counting (RC)** or **Double Reference Counting (DRC)** if a class-level **cyclic** dependency is detected at compile-time.
- Instance of class types (objects) will be released as soon as possible when the object is deemed safe to be removed by RC or DRC.
- A class can extend from (only one) base class but implements multiple traits.
- `@cstruct` annotation can be applied to classes to map the class to a C struct for FFI purpose. For classes, `@cstruct` disallows dynamic dispatching and inheritance.

#### Struct
- Structs are **value** types that live on where they are created.
- A struct can't extend from any other structs but can implement multiple traits.
- Structs can be combined with compile-time annotation `@cunion` to be defined as a C-like union type.
- Structs can be mapped to a C struct using `@cstruct` annotation.

#### Trait
- Traits enforce concrete types to implement specific methods (or properties) but never fields.

#### Enum
- Defines an enumeration type. Enum types can extend from concrete types to provide values to enum members.
```
enum Direction : Int do
    case East  = 0
    case South = 1
    case West  = 2
    case North = 3
end
```

#### Algebraic Data Types
- There are two types of ADTs, value-typed (`enum struct`) and reference-typed (`enum class`).
```
enum struct Optional<T> do
    case None
    case Some(value: T)
end

enum class Tree<T> do
    case Empty
    case Node(value: T, left: Tree<T>, right: Tree<T>)
end

fn foo(o: Optional<Int>) -> Int do
    match o
        case Optional<Int>.Some(value) => value
        case Optional<Int>.None => 0
    end
end
```

#### Alias
- Creates a copy of a type while also allowing adding or hiding methods from the alias type.
- By default, an explicit conversion constructor will be generated, but it can be removed by setting it to `undefined`. Adding `@implicit` to the alias type definition can make the conversion between the original type and its alias implicit (no need for explicit casting).
```
struct Vector2 do
    let x: Real
    let y: Real
    
    fn^ magnitude() -> Real do
        return Math.sqrt(x * x + y * y)
    end
end

alias Point2D : Vector2 do
    constructor(Vector2) = undefined

    fn magnitude() = undefined
end
```

#### Type Extension
- Concrete types can have extended methods or properties through the `extension` statement.
```
class Cat : Animal do
    fn meow() do
        print("meow~")
    end
end

extension CatEx : Cat do
    fn woof() do
        print("woof!")
    end
end

const^ cat = Cat()
cat.woof() // Prints woof!
```
- Type extensions cannot access private or protected members of the type they are extending **unless** the member (or the entire type) is marked with the `@extendible` annotation.

#### Records
- The compile-time annotation `@record` can be applied to concrete types (class, struct, enum, etc.) to **enforce immutability** and automatically generate **equality** and **hash code** implementations.
```
@record
class Point do
    const x: Real
    const y: Real

    constructor(x: Real, y: Real) do
        $x = x
        $y = y
    end
end

const^ a = Point(3, 7)
const^ b = Point(3, 7)
print(a == b) // Prints true
```

#### Operator Overloading
- User-defined types can overload operators to provide custom behaviors. Operator definitions use the `operator` keyword, followed by the operator symbol.
```
struct Vector2 do
    let x: Real
    let y: Real
    
    constructor(x: Real, y: Real) do
        $x = x
        $y = y
    end

    operator +(Vector2 rhs) -> Vector2 do
        return Vector2(x + rhs.x, y + rhs.y)
    end
    
    operator ==(Vector2 rhs) -> Bool do
        return x == rhs.x && y == rhs.y
    end
    
    // ...
end
```
- Operators that are purely syntactic with no runtime behavior such as the immutability operator (`^`) and the user-scope access operator (`$`) cannot be overloaded.


### Module Declaration
- A module is a compile-time scoping mechanism that groups declarations under a shared name.
- Modules do **not** get compiled into runtime objects.
- The body of a module is an execution body. Its top-level statements are executed once the code is processed by the language runtime.
- Names inside a module can only be used inside that module.
```
module Foo do
    fn foo() do
        // ...
    end

    export let s: String = "foo"

    module Bar do
        export fn bar() do
            print(s)
        end
    end
end

Foo.foo() // Compile-time error since `foo` is not exported
Foo.Bar.bar() // Prints "foo"
Foo.s += "bar"
Foo.Bar.bar() // Prints "foobar"
```

#### Nested Module
- When declaring a module, its module name can be written in nested form.
```
module Foo.Bar.Baz do
    // ...
end
```

#### `global` Keyword
- The `global` keyword can be used to access the top-level scope.
```
fn foo() do
    print("foo")
end

module M do
    fn foo() do
        print("bar")
    end
    
    global.foo() // Prints "foo"
end
```

#### Top-level Module
- The top-level scope of a module is seen as an invisible module. To access a name from another file, the name has to be exported.
```
// File A
let foo = "foo"
export let bar = "bar"

// File B
print(foo) // Compile-time error since `foo` in file A is not exported
print(bar) // Prints "bar"
```

#### Importing
- Exported names of a module can be imported into a scope with the `from` and `import` keywords.
```
module ModuleA do
    export class Foo do
        fn foo() do
            // ...
        end
    end
end

module ModuleB do
    from ModuleA import Foo

    const f = Foo()
    f.foo()
end
```
- Multiple names inside a module can be imported together, and each name can be given an alias with the `as` keyword.
```
from Collections import Stack as S, Queue as Q, Map as M
```
- `*` can be used to import all exported names from a module into the current scope.
```
module Collections do
    export class Stack<T> do end
    export class Queue<T> do end
    export class Map<K, V> do end
end

from Collections import *

const s = Stack<Int>()
const q = Queue<Real>()
const m = Map<String, Any>()
```


## Reserved Keywords
- `alias`
- `as`
- `break`
- `case`
- `catch`
- `class`
- `const`
- `constructor`
- `continue`
- `cref`
- `destructor`
- `do`
- `else`
- `end`
- `enum`
- `export`
- `extension`
- `false`
- `fn`
- `for`
- `from`
- `global`
- `if`
- `import`
- `in`
- `is`
- `let`
- `match`
- `module`
- `nil`
- `operator`
- `ref`
- `return`
- `self`
- `struct`
- `super`
- `throw`
- `trait`
- `true`
- `try`
- `typeof`
- `undefined`
- `while`
- `yield`
