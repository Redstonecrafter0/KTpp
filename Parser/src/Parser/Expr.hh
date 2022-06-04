#include <memory>
#include <optional>
#include <vector>

#include "Lexer/Token.hh"

using namespace ktpp::lexer;
namespace ktpp::parser {
class Expr {
 public:
  virtual ~Expr() = default;
};
class GenericParameters;
class Type : public Expr {
 public:
  Token identifier;
  std::unique_ptr<GenericParameters> generics;
  std::optional<std::unique_ptr<Type>> extends;
  Type(Token identifier, std::unique_ptr<GenericParameters> generics,
       std::optional<std::unique_ptr<Type>> extends)
      : identifier(identifier),
        generics(std::move(generics)),
        extends(std::move(extends)) {}
};
class GenericParameter {
 public:
  Token name;
  std::optional<Type> type;
  GenericParameter(Token name, std::optional<Type> type)
      : name(name), type(std::move(type)) {}
};

class GenericParameters {
 public:
  Token LOp;
  std::vector<GenericParameter> type;
  GenericParameters(Token LOp, std::vector<GenericParameter> type = {})
      : LOp(LOp), type(std::move(type)) {}
  bool is_present();
};

class GenericArgs {
 public:
  Token LOp;
  std::vector<Type> params;
  GenericArgs(Token LOp, std::vector<Type> params)
      : LOp(LOp), params(std::move(params)) {}
};
struct Param {
  Token name;
  std::unique_ptr<Type> type;
  Param(Token name, std::unique_ptr<Type> type)
      : name(name), type(std::move(type)) {}
};
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
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> value;
  Assign(Token name, std::unique_ptr<Expr> value)
      : name(name), value(std::move(value)) {}
};
class Call : public Expr {
 public:
  std::unique_ptr<Expr> callee;
  Token paren;
  std::vector<std::unique_ptr<Expr>> args;
  GenericArgs generics;
  Call(std::unique_ptr<Expr> callee, Token paren,
       std::vector<std::unique_ptr<Expr>> args, GenericArgs generics)
      : callee(std::move(callee)),
        paren(paren),
        args(std::move(args)),
        generics(std::move(generics)) {}
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
      : condition(std::move(condition)),
        questionM(questionM),
        ifTrue(std::move(ifTrue)),
        colon(colon),
        ifFalse(std::move(ifFalse)) {}
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
  Array(Token bracket, std::vector<std::unique_ptr<Expr>> values)
      : bracket(bracket), values(std::move(values)) {}
};
class IndexSignature : public Expr {
 public:
  std::unique_ptr<Expr> array, index;
  Token bracket;
  IndexSignature(std::unique_ptr<Expr> array, std::unique_ptr<Expr> index,
                 Token bracket)
      : array(std::move(array)), index(std::move(index)), bracket(bracket) {}
};
class Lambda : public Expr {
 public:
  Token keyword;
  std::unique_ptr<GenericParameters> generics;
  std::vector<Param> params;
  std::unique_ptr<Expr> expr;
  std::optional<std::unique_ptr<Type>> type;
  Lambda(Token keyword, std::unique_ptr<GenericParameters> generics,
         std::vector<Param> params, std::optional<std::unique_ptr<Type>> type,
         std::unique_ptr<Expr> expr)
      : keyword(keyword),
        generics(std::move(generics)),
        params(std::move(params)),
        type(std::move(type)),
        expr(std::move(expr)) {}
  bool is_generic();
};
}  // namespace ktpp::parser