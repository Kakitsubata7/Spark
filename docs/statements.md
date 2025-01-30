# Statements

## Statement Tree
- `do`
  - `<statement>*`
    - `end`
  - `end`
- `if`
    - `<expression>` (must evaluate to `bool`)
      - `do`
        - `<statement>*`
          - `end`
        - `end`

## Declarations

### `let`

### `const`

### `fn`

## Lexical Scope
Use `do` and `end` keywords to define a lexical scope. This statement is commonly associated with other keywords to define different types of lexical scopes including function scope, class scope, module scope, etc.
```spark
// Outside of the scope
do
    // Inside of the scope
end
// Outside of the scope
```
Note that `do` strictly indicates the start of a lexical scope. However `end` is just used to indicate the end of something. `do` and `end` are not always used together. `end` can also be used to separate 

## Control Flow Statements
### `if, else if, else`
```spark
if condition do
    // ...
end
else if condition do
    // ...
end
else do
    // ...
end
```

### `do-if`
Is a syntax sugar of the `if` statement. It's better to be used with a single statement for clarity.
```spark
do /* ... */ if condition

do
    // ...
if condition
```

### Ternary
```spark
if condition do /* ... */ else /* ... */ 
```

### `match-case`
Pattern matching statement. Can be used similar to `switch` statement from *C*-style languages (*C/C++*, *Java*, etc.). For example:
```spark
match x
    case 0 do /* ... */ end     // Equal case
    case x > 0 do /* ... */ end // Conditional case
    case _ do /* ... */ end     // Default case
end
```
`match-case` can also be used to extract elements from tuples, objects (class instances), struct and union instances:
```spark
// Tuple
const t = ("pi", 3.14)
match t
    case (s: String, n: Real) do // Extraction case
        /* ... */
    end
end

// Object
const obj^ = MyClass(name="Foo")
match obj
    case {name: String} do
        // Extracts 'name' field from obj if 'name' is String
    end
end

// Struct
const v2 = Vector2(x=0.1, y=2.3)
match obj
    case {x, y} do
        // Extracts 'x' and 'y'. Types are automatically inferred 
    end
end

// Union
const u = MyUnion()
// 'MyUnion' is a union of types Int, Real, and String
match u
    case i: Int do /* ... */ end
    case n: Real do /* ... */ end
    case s: String do /* ... */ end
    
    case _ do /* ... */ end
    // Warning since pattern maching is exhauted before this case
end
```
This extraction behavior doesn't seem to be too useful with known types, but it also works with generic types:
```spark
fn foo<T>(x: T) do
    match x
        case x is Real do
            // The type of x here is considered as Real by
            // the compiler
        end
        case x is MyClass do
            // The type of x here is considered as MyClass
            // by the compiler
            x.someMethod()
        end
        case {name: String} do
            // x has a field named 'name' with type String
        end
        case {bar} do
            // x has a field named 'bar' with an unknown
            // type
        end
        case _ do
            // Some default behaviors
        end
    end
end
```
The *Spark* compiler will remove any unreachable cases. For example, `foo<Int>` will be compiled to something equivalent to this function:
```spark
fn foo(x: Int) do
    // Some default behaviors, because it's the only
    // reachable case
end
```
