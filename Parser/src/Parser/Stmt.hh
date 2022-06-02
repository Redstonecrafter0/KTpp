#include "../../../Lexer/src/Lexer/Token.hh"
#include "Expr.hh"
#include <variant>
#include <vector>

using namespace ktpp::lexer;

namespace ktpp::parser {
class Stmt {};
class Scope : public Stmt {
public:
  std::vector<std::unique_ptr<Stmt>> statements;
  Scope(std::vector<std::unique_ptr<Stmt>> statements)
      : statements(statements) {}
};
class Expression : public Stmt {
public:
  std::unique_ptr<Expr> expr;
  Expression(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}
};
class If : public Stmt {
public:
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> thenBranch;
  std::optional<std::unique_ptr<Stmt>> elseBranch;
  If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch,
     std::optional<std::unique_ptr<Stmt>> elseBranch)
      : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}
};
class Return : public Stmt {
public:
  std::optional<std::unique_ptr<Expr>> value;
  Token keyword;
  Return(Token keyword, std::optional<std::unique_ptr<Expr>> value)
      : keyword(keyword), value(std::move(value)) {}
};
class Var : public Stmt {
public:
  Token name;
  std::optional<std::unique_ptr<Expr>> initializer;
  std::optional<std::unique_ptr<Type>> type;
  bool isMut;
  Var(Token name, std::optional<std::unique_ptr<Expr>> initializer,
      std::optional<std::unique_ptr<Type>> type, bool isMut = false)
      : name(name), initializer(std::move(initializer)),
        type(type.has_value() ? std::make_optional(std::move(type.value()))
                              : std::nullopt),
        isMut(isMut) {}
};
class While : public Stmt {
public:
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> statement;
  bool isDoWhile;
  While(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> statement,
        bool isDoWhile = false)
      : condition(std::move(condition)), statement(std::move(statement)),
        isDoWhile(isDoWhile) {}
};
class For : public Stmt {
public:
  std::optional<std::unique_ptr<Stmt>> init;
  std::unique_ptr<Expr> condition, increment;
  std::unique_ptr<Stmt> statement;
  For(std::optional<std::unique_ptr<Stmt>> init,
      std::unique_ptr<Expr> condition, std::unique_ptr<Expr> increment,
      std::unique_ptr<Stmt> statement)
      : init(std::move(init)), condition(std::move(condition)),
        increment(std::move(increment)), statement(std::move(statement)) {}
};
class ForEach : public Stmt {
public:
  std::unique_ptr<Stmt> init;
  Token colon;
  std::unique_ptr<Expr> iterator;
  std::unique_ptr<Stmt> statement;
  ForEach(std::unique_ptr<Var> init, Token colon,
          std::unique_ptr<Expr> iterator, std::unique_ptr<Stmt> statement)
      : init(std::move(init)), colon(colon), iterator(std::move(iterator)),
        statement(std::move(statement)) {}
};
class Break : public Stmt {
public:
  Token keyword;
  Break(Token keyword) : keyword(keyword) {}
};
class Continue : public Stmt {
public:
  Token keyword;
  Continue(Token keyword) : keyword(keyword) {}
};
struct Param {
  Token name;
  std::unique_ptr<Type> type;
  Param(Token name, std::unique_ptr<Type> type)
      : name(name), type(std::move(type)) {}
};
class Function : public Stmt {
public:
  Token name;
  std::optional<std::vector<Generic>> generics;
  std::optional<std::vector<Param>> params;
  std::optional<std::unique_ptr<Type>> type;
  Scope body;
  Function(Token name, std::optional<std::vector<Generic>> generics,
           std::optional<std::vector<Param>> params,
           std::optional<std::unique_ptr<Type>> type, Scope body)
      : name(name), generics(generics), params(params),
        type(type.has_value() ? std::make_optional(std::move(type.value()))
                              : std::nullopt),
        body(std::move(body)) {}
};
class Class : public Stmt {
public:
  Token name;
  std::vector<std::unique_ptr<Var>> fields;
  std::vector<Function> methods;
  std::optional<std::vector<Generic>> generics;
  std::optional<std::unique_ptr<Expr>> superclass;
  Class(Token name, std::vector<std::unique_ptr<Var>> fields,
        std::vector<Function> methods,
        std::optional<std::unique_ptr<Expr>> superclass,
        std::optional<std::vector<Generic>> generics)
      : name(name), fields(std::move(fields)), methods(methods),
        superclass(std::move(superclass)), generics(generics) {}
};
template <typename T> struct SwitchCase {
  std::unique_ptr<Expr> value;
  T Case;
  Token op;
  SwitchCase(std::unique_ptr<Expr> value, T x, Token op)
      : value(value), x(x), op(op) {}
};
template <typename T> class Switch : public Stmt {
public:
  std::unique_ptr<Expr> condition;
  std::vector<SwitchCase<T>> cases;
  std::optional<T> defaultCase;
  Switch(std::unique_ptr<Expr> condition, std::vector<SwitchCase<T>> cases,
         std::optional<T> defaultCase)
      : condition(condition), cases(cases), defaultCase(defaultCase) {}
};
} // namespace ktpp::parser