#include <vector>

namespace ktpp::parser {
class Expr {};
class Literal : public Expr {
public:
  std::any value;
  Literal(std::any value) : value(value) {}
};
class Binary : public Expr {
public:
  Expr left, right;
  lexer::Token op;
  Binary(Expr left, lexer::Token op, Expr right)
      : left(left), op(op), right(right) {}
};
class Unary : public Expr {
public:
  Expr left;
  lexer::Token op;
  Unary(Expr left, lexer::Token op) : left(left), op(op) {}
};
class Grouping : public Expr {
public:
  Expr expr;
  Grouping(Expr expr) : expr(expr) {}
};
class Variable : public Expr {
public:
  lexer::Token name;
  Variable(lexer::Token name) : name(name) {}
};
class Assign : public Expr {
public:
  lexer::Token name;
  Expr value;
  Assign(lexer::Token name, Expr value) : name(name), value(value) {}
};
class Call : public Expr {
public:
  Expr callee;
  lexer::Token paren;
  std::vector<Expr> args;
  std::vector<lexer::Token> generics;
  Call(Expr callee, lexer::Token paren, std::vector<Expr> args,
       std::vector<lexer::Token> generics)
      : callee(callee), paren(paren), args(args), generics(generics) {}
};
class Get : public Expr {
public:
  Expr object;
  lexer::Token name;
  Get(Expr object, lexer::Token name) : object(object), name(name) {}
};
class Set : public Expr {
public:
  Expr object;
  lexer::Token name;
  Expr value;
  Set(Expr object, lexer::Token name, Expr value)
      : object(object), name(name), value(value) {}
};
class Ternary : public Expr {
public:
  Expr condition, ifTrue, ifFalse;
  lexer::Token questionM;
  Ternary(Expr condition, Expr ifTrue, Expr ifFalse, lexer::Token questionM)
      : condition(condition), ifTrue(ifTrue), ifFalse(ifFalse),
        questionM(questionM) {}
};
class This : public Expr {
public:
  lexer::Token keyword;
  This(lexer::Token keyword) : keyword(keyword) {}
};
class Array : public Expr {
public:
  lexer::Token bracket;
  std::vector<Expr> values;
  Array(lexer::Token bracket, std::vector<Expr> values)
      : bracket(bracket), values(values) {}
};
class IndexSignature : public Expr {
public:
  Expr array, index;
  lexer::Token bracket;
  IndexSignature(Expr array, Expr index, lexer::Token bracket)
      : array(array), index(index), bracket(bracket) {}
};
class Type : public Expr {
public:
  lexer::Token identifier;
  bool isGeneric;
  std::optional<std::vector<Type>> generics;
  Type(lexer::Token identifier, std::optional<std::vector<Type>> generics = {})
      : identifier(identifier), isGeneric(generics.has_value()),
        generics(generics) {}
};
} // namespace ktpp::parser