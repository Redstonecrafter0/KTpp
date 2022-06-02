#include <optional>
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
  std::unique_ptr<Expr> left, right;
  Token op;
  Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
      : left(std::move(left)), op(op), right(std::move(right)) {}
};
class Unary : public Expr {
public:
  std::unique_ptr<Expr> right;
  Token op;
  Unary(std::unique_ptr<Expr> right, Token op)
      : right(std::move(right)), op(op) {}
};
class Grouping : public Expr {
public:
  std::unique_ptr<Expr> expr;
  Grouping(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}
};
class Variable : public Expr {
public:
  Token name;
  Variable(Token name) : name(name) {}
};
class Assign : public Expr {
public:
  Token name;
  std::unique_ptr<Expr> value;
  Assign(Token name, std::unique_ptr<Expr> value)
      : name(name), value(std::move(value)) {}
};
class Call : public Expr {
public:
  std::unique_ptr<Expr> callee;
  Token paren;
  std::vector<std::unique_ptr<Expr>> args;
  std::vector<Token> generics;
  Call(std::unique_ptr<Expr> callee, Token paren,
       std::vector<std::unique_ptr<Expr>> args, std::vector<Token> generics)
      : callee(std::move(callee)), paren(paren), args(args),
        generics(generics) {}
};
class Get : public Expr {
public:
  std::unique_ptr<Expr> object;
  Token name;
  Get(std::unique_ptr<Expr> object, Token name)
      : object(std::move(object)), name(name) {}
};
class Set : public Expr {
public:
  std::unique_ptr<Expr> object;
  Token name;
  std::unique_ptr<Expr> value;
  Set(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value)
      : object(std::move(object)), name(name), value(std::move(value)) {}
};
class Ternary : public Expr {
public:
  std::unique_ptr<Expr> condition, ifTrue, ifFalse;
  Token questionM, colon;
  Ternary(std::unique_ptr<Expr> condition, Token questionM,
          std::unique_ptr<Expr> ifTrue, Token colon,
          std::unique_ptr<Expr> ifFalse)
      : condition(std::move(condition)), questionM(questionM),
        ifTrue(std::move(ifTrue)), colon(colon), ifFalse(std::move(ifFalse)) {}
};
class This : public Expr {
public:
  Token keyword;
  This(Token keyword) : keyword(keyword) {}
};
class Array : public Expr {
public:
  Token bracket;
  std::optional<std::vector<std::unique_ptr<Expr>>> values;
  Array(Token bracket, std::optional<std::vector<std::unique_ptr<Expr>>> values)
      : bracket(bracket), values(values) {}
};
class IndexSignature : public Expr {
public:
  std::unique_ptr<Expr> array, index;
  Token bracket;
  IndexSignature(std::unique_ptr<Expr> array, std::unique_ptr<Expr> index,
                 Token bracket)
      : array(std::move(array)), index(std::move(index)), bracket(bracket) {}
};
class Generic : public Expr {
public:
  Token LOp;
  std::map<Token, std::optional<Type>> type;
  Generic(Token LOp, std::map<Token, std::optional<Type>> type)
      : LOp(LOp), type(type) {}
};
class Type : public Expr {
public:
  Token identifier;
  bool isGeneric;
  std::optional<std::vector<Generic>> generics;
  std::optional<std::unique_ptr<Type>> extends;
  Type(Token identifier, std::optional<std::vector<Generic>> generics,
       std::optional<std::unique_ptr<Type>> extends)
      : identifier(identifier), isGeneric(generics.has_value()),
        generics(generics), extends(std::move(extends)) {}
};
class Lambda : public Expr {
public:
  Token keyword;
  bool isGeneric;
  std::optional<std::vector<Generic>> generics;
  std::optional<std::unique_ptr<Expr>> expr;
  std::optional<std::unique_ptr<Type>> type;
  Lambda(Token keyword, std::optional<std::vector<Generic>> generics,
         std::optional<std::vector<Param>>,
         std::optional<std::unique_ptr<Type>> type,
         std::optional<std::unique_ptr<Expr>> expr)
      : keyword(keyword), isGeneric(generics.has_value()), generics(generics),
        type(type.has_value() ? std::make_optional(std::move(type.value()))
                              : std::nullopt),
        expr(expr.has_value() ? std::make_optional(std::move(expr.value()))
                              : std::nullopt) {}
};
} // namespace ktpp::parser