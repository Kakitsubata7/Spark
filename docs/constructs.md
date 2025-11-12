### Literals `<expr>`
```
Γ ⊢ <integer> : Int   (T-Int)
Γ ⊢ <real> : Real     (T-Real)
Γ ⊢ <string> : String (T-String)
Γ ⊢ true : Bool       (T-True)
Γ ⊢ false : Bool      (T-False)
```

### Array `<expr>`
```
Γ ⊢ a : A, b : B, c : C
----------------------------------- (T-Array)
Γ ⊢ { a, b, c } : Array<A | B | C>
```

### Variables `<expr>`
```
Γ(x) = T
---------- (T-Var)
Γ ⊢ x : T
```

### Sequence `<expr> <stmt>`
```
Γ ⊢ a : A
Γ ⊢ b : B
--------------
Γ ⊢ a ; b : B
```

```
Γ ⊢ x : T
------------
Γ ⊢ x ; : T
```

### Variable Definitions `<stmt>`
```
Γ ⊢ e : T
-------------------------------
Γ, x : T ⊢ let x: T = e : Void
```

```
Γ ⊢ e : T
----------------------------
Γ, x : T ⊢ let x = e : Void
```

```
Γ ⊢ e : T
---------------------------------
Γ, x : T ⊢ const x: T = e : Void
```

```
Γ ⊢ e : T
------------------------------
Γ, x : T ⊢ const x = e : Void
```

### Function Expressions `<expr>`
```
Γ, x : Targ ⊢ S : Void
----------------------------------------------------
Γ ⊢ fn (arg : Targ) -> Tret do S end : Targ -> Tret
```

```
Γ, x : Targ ⊢ S : Void
Terr <: Error
--------------------------------------------------------------------------
Γ ⊢ fn (arg : Targ) -> Tret throw Terr do S end : Targ -> Tret throw Terr
```

```
Γ, x : Targ ⊢ e : Tret
------------------------------------------------
Γ ⊢ fn (arg : Targ) -> Tret => e : Targ -> Tret
```

```
Γ, x : Targ ⊢ e : Tret
Terr <: Error
----------------------------------------------------------------------
Γ ⊢ fn (arg : Targ) -> Tret throw Terr => e : Targ -> Tret throw Terr
```

### Function Definitions `<stmt>`
```
Γ, f : Targ -> Tret, x : Targ ⊢ S : Void
---------------------------------------------
Γ ⊢ fn f(arg : Targ) -> Tret do S end : Void
```

```
Γ, f : Targ -> Tret throw Terr, x : Targ ⊢ S : Void
Terr <: Error
--------------------------------------------------------
Γ ⊢ fn f(arg : Targ) -> Tret throw Terr do S end : Void
```

```
Γ, f : Targ -> Tret, x : Targ ⊢ e : Tret
-----------------------------------------
Γ ⊢ fn f(arg : Targ) -> Tret => e : Void
```

```
Γ, f : Targ -> Tret throw Terr, x : Targ ⊢ e : Tret
Terr <: Error
----------------------------------------------------
Γ ⊢ fn f(arg : Targ) -> Tret throw Terr => e : Void
```

### If-Else `<stmt>`
```
Γ ⊢ b : Bool
Γ ⊢ S1 : Void
Γ ⊢ S2 : Void
-----------------------------------------
Γ ⊢ if b do S1 end else do S2 end : Void
```

### If-Then `<expr>`
```
Γ ⊢ b : Bool
Γ ⊢ x : T1
Γ ⊢ y : T2
---------------------------------
Γ ⊢ if b then x else y : T1 | T2
```

### Try-Else `<expr>`
```
Γ ⊢ x : T1
Γ ⊢ y : T2
---------------------------
Γ ⊢ try x else y : T1 | T2
```

### Throw `<expr> <stmt>`
```
Γ ⊢ e : E
E <: Error
------------------
Γ ⊢ throw e : Any
```
