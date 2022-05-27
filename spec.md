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

## Variable decleration (decl)

```
Infered type:
var x = <expr>
Explicit type:
var x: Type = <expr>
Mutable decleration:
var mut x = <expr>

General:
var [mut] <identifier>[: Type] = <expr>
```

## Function decleration (func)

```
No return value no arguments:
fn a <expr>

fn a() <expr>

No arguments, with return value:

fn a -> Type <expr>

fn a() -> Type <expr>

With arguments, with return value:

fn a(b: Type, c: Type) -> Type <expr>

With generics:

fn a<K : Type, V> <expr>

General:
fn identifier [<identifier[: Type]...>] [([identifier: Type]...)] [-> Type] <expr>
```

## Function type decleration (fntype):

```
As function without identifiers:

type FuncType = fn -> int

Usage:
fn a(b: fn <T>(T, int) -> int) -> FuncType

General:
fn [<[identifier[: Type]]>] [([Type]...)] [-> Type]
```

## Labda expressions (lambda)

```
Usage:

fn a(b: fn(int, int) -> int) b(1, 2)

a(fn (a, b) a + b)

General:
fn [<identifier[: Type]...>] [(identifier[: Type]...)] [-> Type] <expr>
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

class Identifier [: Superclass] {
  [[pub] identifier[: Type] [= <expr>]]...

  [init[(identifier[: Type]...)] {
    <expr>
  }]

  [copy(from: &Identifer) {
    <expr>
  }]

  [destroy {
    <expr>
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

interface Identifier[<[identifier[: Type]]...>] [: Supertype [, Supertype]...] {
  fn identifier [<identifier[: Type]...>] [([identifier: Type]...)] [-> Type]
}
```

## Function call (call)

```
Usage:
function<Type, Type>(arg1, arg2)

General:
identifier[<Type...>]([<expr>]...)
```

## Assignments (assign)

```
Usage:
identfier = <expr>
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
Usage
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
[
  -
  | ~
  | !
  | (Type)
  <expr>
] | [
  <expr>
  '[' <expr> ']'
  | ++
  | --
  | as Type
]
```

## Ternary operator (ternary)

```
Usage:
x ? y : z

General:
<expr> ? <expr> : <expr>
```

## Member selection (member)

```
Usage:

x.y
y.z()

General:
<expr>[.<expr>]...
```

## Type alias (type)

```
Usage:
type Z = X<A>

General:
type <expr> = <expr>
```

## For loop (for)

```
Usage
for start; stop; step {
  doStuff()
}

for var a in iteratable {
  doStuff(a)
}

General:
for <expr>; <expr>; <expr> <expr>
| for <expr> in <expr> <expr>
```

## While

```
Usage
while condition {
  doStuff()
}

General:
while <expr> <expr>
```

## If

```
Usage

if condition {
  doStuff()
}

General:
if <expr> <expr>
```

# Methods

## With mut

A method with `mut` is only callable by variables of type `mut Type`.
If a method is declared as `mut`, `this` and all members are mutable.

# Pub

Any Type, Interface, Global, Class, Method, Member and Function may be exposed with
`pub` and is from then on reachable from everywhere. Otherwise everything is package private.
