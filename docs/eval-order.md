## Evaluation Order

### Function Calls
- Arguments are evaluated from **left to right** before the call is made. After all arguments are evaluated, the function body executes.

### Function Return
- The returned expression is evaluated before leaving the function.

### Variable/Reference Declarations
- Syntax: `let/const/ref/cref <identifier> = <expr>`
- The expression on the right-hand side is evaluated first, then the value is bound to the identifier.

### Assignment
- The left-hand side is first resolved to a location, then the right-hand side is evaluated. Finally, the assignment operation is performed.

### Unary Operators
- The operand of a unary operator is evaluated first, then the unary operator is applied.

### Binary Operators
- The left operand is evaluated first, then the right operand is evaluated.
- Logical binary operators (`&&` and `||`) use **short-circuit evaluation**.
  - For `a && b`, if `a` evaluates to `false`, `b` will not be evaluated.
  - For `a || b`, if `a` evaluates to `true`, `b` will not be evaluated.

### If Statement
- Syntax:
```
if <expr> do
    <statement>*
end else if <expr> do
    <statement>*
end else do
    <statement>*
end
```
- The condition expression of the first `if` is evaluated first. If it evaluates to `true`, the statements in its block are executed and the rest are skipped. If the condition is false, each `else if` condition expression is evaluated in order until one is `true` so that its block is executed and the rest are skipped. If none are `true` and an `else` block is present, that block is executed.

### While Loop
- Syntax: `while <expr> do <statement>* end`
- The condition expression is evaluated before each iteration. If the condition is `true`, the loop body executes, then the next iteration starts with re-evaluating the condition expression. If the condition is `false`, the loop terminates.

### For Loop
- Syntax: `for <identifier> in <expr> do <statement>* end`
- The expression after `in` is evaluated first. For each element the expression produces, the identifier is bound to that element and the loop body executes.

### Match
- Syntax:
```
// `match` expression
match <expr> as <identifier> // `as <identifier>` is optional
    case <expr> => <expr>
    case if <expr> => <expr>
    case _ => throw <expr>
end

// `match` statement
match <expr> as <identifier>
    case <expr> do <statement>* end
    case if <expr> do <statement>* end
    case _ do <statement>* end
end
```
- The expression after `match` is evaluated first.
- Then each `case` is then checked from top to bottom:
  - For a `case <expr>`, the expression is evaluated and compared against the matched value.
  - For a `case if <expr>`, the boolean condition expression is evaluated. If it is `true`, the case is matched.
  - For a `case _`, it always matches.
- Once a case matches, its body (`=> <expr>` or `do <statement>* end`) is evaluated and other cases are skipped.

### Throw
- The expression being thrown is evaluated before throwing.

### `try/else`
- Syntax: `try <expr> else <expr>` or `try <expr> else throw <expr>`
- The expression before `else` is evaluated first. The expression after `else` is only evaluated if the previous expression throws an error.
- If the part after `else` is a `throw` statement, the expression after `throw` is evaluated before throwing.

### `try/catch`
- Syntax:
```
try do
    <statement>*
end catch <type> do
    <statement>*
end catch <type> as <identifier> do
    <statement>*
end catch _ do
    <statement>*
end ...
```
- The statements in the try block are executed sequentially. If an error is thrown, the remaining statements in the try block are skipped. The thrown error is then matched against the catch clauses in order, from top to bottom. The first matching catch is selected (_ matches any error), and its statements are executed.
