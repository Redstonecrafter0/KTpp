# Stuff wed like

- Manual memory management
- Union types `int | string`
- Ducktyping

# For OOP

- Constructor (init)
- Copy constructor (copy)
- Destructor (destroy)

# Primitives

## Numbers

Signed integer: i8, i16, i32, i64  
Unsigned integer: u8, u16, u32, u64 + usize, isize (Architecture specific size)  
Floating point data types: f32 single precision, f64 double precision

## String

Default representation is in a wrapper.
String wrapper:

- Len: usize
- Cap: usize
- Ptr: u8[]

## Arrays

Representation is in a wrapper.
Array wrapper:

- Len: usize
- Cap: usize
- Ptr: T[]

## Boolean

Either `true` or `false` language type `bool`

# Syntax

## Identifier (id)

```
General:
[a-Z|_][a-Z|0-9|_]...
```

## Expression (expr)

```
Expresion is syntax which yields a value.
<expr Type> must yield Type.

General:
<switch> | <call> | <lambda> | <int> | <float> | <string> | <bool>
| <arr> | <map> | <binop> | <unop> | <ternary>
```

## Statement (stmt)

```
Statement is any executable amount of code.
Might be a scope aswell.

General:
<scope> | <if> | <for> | <while> | <expr> | <decl> | <assign>
```

## Scope (scope)

```
General:
{
  [<stmt>]...
}
```

## Variable declaration (decl)

```

Infered type:
var x = <expr>
Explicit type:
var x: Type = <expr>
Mutable declaration:
var mut x = <expr>
Constant decleration:
val x = <expr>

General:
Var:
var [mut] <id>[: Type] = <expr>
Val:
val <id>[: Type] = <expr>

```

## Generic parameter (genericparam)

```

Not valid standing alone
General:
['<'[<id>[: <typespec>],]...'>']

```

## Generic arguments (genericargs)

```

Not valid standing alone
General:
['<'[<typespec>,]...'>']

```

## Parameter list (params)

```

Not valid standing alone
General:
Typed:
([<id>: Type,]...)
Untyped:
([<id>[: Type],]...)

```

## Function declaration (func)

```

No return value no arguments:
fn a <stmt>

fn a() <stmt>

No arguments, with return value:

fn a -> Type <stmt>

fn a() -> Type <stmt>

With arguments, with return value:

fn a(b: Type, c: Type) -> Type <stmt>

With generics:

fn a<K : Type, V> <stmt>

General:
fn identifier <genericparams> [<params typed>] [-> Type] <stmt>

```

## Function type declaration (fntype):

```

As function without identifiers:

type FuncType = fn -> int

Usage:
fn a(b: fn <T>(T, int) -> int) -> FuncType

General:
fn <genericparams> [([Type]...)] [-> Type]

```

## Lambda expressions (lambda)

```

Usage:

var a = fn (b: fn(int, int) -> int) b(1, 2)

a(fn (a, b) a + b)

General:
fn <genericparams> [<params untyped>] [-> Type] <stmt>

```

## Class definition (class)

```

Without generics/constructor

class Identfier {

}

With generics/constructor

class Identifier<K : Type, V> {
  init(a: Type) {
  }

  copy(from: Type) {
  }

  destroy {
  }
}

Member values/methods

class Identifier : Superclass {
  identifier = value  Direclty initialized
  identifier: Type    Initialized in init

  fn identifier() -> Type this.identifier

  mut fn identifier(a: Type) {
    this.identifier = a
  }
}

General:

class Identifier <genericparams> [: Superclass] {
  [[pub] <id>[: Type] [= <expr>]]...

  [init[<params typed>] {
    <stmt>
  }]

  [copy(from: &Identifer) {
    <stmt>
  }]

  [destroy {
    <stmt>
  }]

  [[pub] [mut] <func>]...
}

Access to superclass with the `super` keyword
Access to the current instance in a member function with the `this` keyword

```

## Interface definition (interface)

```

Usage:

interface Identifier<K : Type, V> : Superinterface {
  mut fn a(x: int, y: int) -> int
}

General:

interface Identifier['<'[<id>[: Type]]...'>'] [: Supertype [, Supertype]...] {
  [fn <id> [<genericparams>] [<params typed>] [-> Type]]...
}

```

## Function call (call)

```

Usage:
function<Type, Type>(arg1, arg2)

General:
<expr>[<genericargs>]([<expr>]...)

```

## Assignments (assign)

```

Usage:
x = 1

General:
<id> = <expr>

```

## Array literals (arr)

```

Usage:
[new Car(), new Horse()]

General:
'['[<expr>][,<expr>]...[,]']'

```

## Map literal (map)

```

Usage
{
  key: value,
  bob: marley
}

General:
{[<expr>: <expr>][,<expr>: <expr>]...[,]}

```

## String literal (string)

```

Usage:
"text"

General:
'"'<text>'"'

```

## Integer literal (int)

```

Usage:
100
0x1a0
0b100
0o100
1_000
10e10

General:
[0x[0-f|_]...] | [0b[0|1|_]...] | [0o[0-8|_]...] | [[0-9|_]...[e[0-9]...]]

```

## Float literal (float)

```

Usage:
123.456
123_456e-3

General:
[[0-9|_]...[[-]e[0-9]...]]

```

## Bool literal (bool)

```

Usage:
true
false

General:
true|false

```

## Binary operations (binop)

```

Usage:
1 + 2
a += b

General
<expr>
[
  '||'
  | &&
  | ==
  | !=
  | >=
  | >
  | <
  | <=

] | [

  <<
  | >>
  | '|'
  | &
  | ^
  | +
  | -
  | *
  | /
  | %
  | **
[=]
]
<expr>

```

## Unary operator (unop)

```

Usage:
-1
~b

General:

  -
  | ~
  | !
  | (Type)
  <expr>
|
  <expr>
  '[' <expr> ']'
  | ++
  | --
  | as Type

```

## Ternary operator (ternary)

```

Usage:
x ? y : z

General:
<expr bool> ? <expr> : <expr>

```

## Member selection (member)

```

Usage:

x.y
y.z()

General:
<expr>[.<expr>]...

```

## Typespec (type)

```

Only top level may be declared mutable.
No generic can be declared mutable.

General:
[&][mut] <id><genericargs>

```

## Type alias (alias)

```

Usage:
type Z = X<A>

General:
type <id>[<genericparams>] = <typespec>

```

## For loop (for)

```

Usage:
for start; stop; step {
  doStuff()
}

for var a in iteratable {
  doStuff(a)
}

General:
for <stmt>; <expr bool>; <stmt> <stmt>
| for var <id> in <expr> <stmt>

```

## While (while)

```

Usage:
while condition {
  doStuff()
}

do while condition {
  doStuff()
}

General:
[do] while [<expr bool>] <stmt>

```

## If (if)

```

Usage:
if condition {
  doStuff()
}

General:
if <expr bool> <stmt>

```

## Switch (switch)

```

Usage:
switch a {
  1 -> aIsOne()
  b*2 -> aIsTwiceB()
  _ -> unsure()
}

General:

switch <expr> {
  [case <expr>: <stmt>]...
  [default: <stmt>]
}
|
switch <expr> {
  [<expr> -> <stmt>]...
  [_ -> <stmt>]
}

```

## File layout

```

Every file generally follows this layout:

package <id>[.<id>]...

[import <id>[.<id>]...]...

[<class> | <func> | <decl val>]...

```

# Behaviour
## Methods

### With mut

A method with `mut` is only callable by variables of type `mut Type`.
If a method is declared as `mut`, `this` and all members are mutable.

## Pub

Any Type, Interface, Global, Class, Method, Member and Function may be exposed with
`pub` and is from then on reachable from everywhere. Otherwise everything is package private.

## Typing

### Ownership

Values assigned to a variable are owned by the variable.
This is also valid for array indecies and members.

### Mutability

Only class member or array indecies of a mutable value may be reassigned.
Immutable local variables may be reassigned because the value is Immutable and not the variable.
Therefore mutabilty is part of the typespec.

### Reference

Types defined with a leading `&` are references to values.
Values may be passed by reference to functions.
References may be assigned to locals.
Ownership cannot be taken of references.

### Var vs Val

Locals declared with var are variables which may be redefined.
Variables may be mutable.
Locals or Globals defined with val are values which may not be redefined.
Values are immutable.
