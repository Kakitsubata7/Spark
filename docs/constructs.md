### Literals `<expr>`
```
Γ ⊢ <integer> : Int
Γ ⊢ <real> : Real
Γ ⊢ <string> : String
Γ ⊢ true : Bool
Γ ⊢ false : Bool
```

### Array `<expr>`
```
Γ ⊢ a : A, b : B, c : C
----------------------------------
Γ ⊢ { a, b, c } : Array<A | B | C>
```

### Variables `<expr>`
```
Γ(x) = T
---------
Γ ⊢ x : T
```

### Sequence `<expr> <stmt>`
```
Γ ⊢ a : A
Γ ⊢ b : B
----------------
Γ ⊢ a ; b : B
```

### If-Else `<stmt>`
```
Γ ⊢ b : Bool
Γ ⊢ S1 : Void
Γ ⊢ S2 : Void
---------------------------------------
Γ ⊢ if b do S1 end else do c end : Void
```

### If-Then `<expr>`
```
Γ ⊢ a : Bool
Γ ⊢ b : B
Γ ⊢ c : C
------------------------------
Γ ⊢ if a then b else c : B | C
```

### Try-Else `<expr>`
```
Γ ⊢ a : A
Γ ⊢ b : B
------------------------
Γ ⊢ try a else b : A | B
```

### Throw `<expr> <stmt>`
```
Γ ⊢ e : E
E <: Error
------------------
Γ ⊢ throw e : Any
```
