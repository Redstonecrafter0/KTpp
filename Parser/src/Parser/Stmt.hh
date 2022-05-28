#include "Expr.hh"
#include "Lexer/Token.hh"
#include <variant>
#include <vector>

namespace ktpp::parser {
class Stmt {};
class Block : public Stmt {
public:
  std::vector<Stmt> statements;
  Block(std::vector<Stmt> statements) : statements(statements) {}
};
class Expression : public Stmt {
public:
  Expr expr;
  Expression(Expr expr) : expr(expr) {}
};
class If : public Stmt {
public:
  Expr condition;
  Stmt thenBranch;
  std::optional<Stmt> elseBranch;
  If(Expr condition, Stmt thenBranch, std::optional<Stmt> elseBranch)
      : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}
};
class Return : public Stmt {
public:
  std::optional<Expr> value;
  lexer::Token keyword;
  Return(lexer::Token keyword, std::optional<Expr> value)
      : keyword(keyword), value(value) {}
};
class Var : public Stmt {
public:
  lexer::Token name;
  std::optional<Expr> initializer;
  bool isMut;
  Var(lexer::Token name, std::optional<Expr> initializer, bool isMut = false)
      : name(name), initializer(initializer), isMut(isMut) {}
};
class While : public Stmt {
public:
  Expr condition;
  Stmt statement;
  bool isDoWhile;
  While(Expr condition, Stmt statement, bool isDoWhile = false)
      : condition(condition), statement(statement), isDoWhile(isDoWhile) {}
};
class For : public Stmt {
public:
  std::optional<Stmt> init;
  Expr condition;
  Expr increment;
  Stmt statement;
  For(std::optional<Stmt> init, Expr condition, Expr increment, Stmt statement)
      : init(init), condition(condition), increment(increment),
        statement(statement) {}
};
class ForEach : public Stmt {
public:
  Var init;
  lexer::Token colon;
  Stmt statement;
  Expr iterator;
  ForEach(Var init, lexer::Token colon, Expr iterator, Stmt statement)
      : init(init), colon(colon), iterator(iterator), statement(statement) {}
};
class Break : public Stmt {
public:
  lexer::Token keyword;
  Break(lexer::Token keyword) : keyword(keyword) {}
};
class Continue : public Stmt {
public:
  lexer::Token keyword;
  Continue(lexer::Token keyword) : keyword(keyword) {}
};
struct Param {
  lexer::Token name;
  Type type;
  Param(lexer::Token name, Type type) : name(name), type(type) {}
};
class Function : public Stmt {
public:
  lexer::Token name;
  Type type;
  std::optional<std::vector<lexer::Token>> generics;
  std::optional<std::vector<Param>> params;
  Block body;
  Function(lexer::Token name, std::optional<std::vector<lexer::Token>> generics,
           std::optional<std::vector<Param>> params, Block body, Type type)
      : name(name), params(params), body(body), generics(generics), type(type) {
  }
};
class Class : public Stmt {
public:
  lexer::Token name;
  std::vector<Function> methods;
  std::optional<std::vector<lexer::Token>> generics;
  std::optional<Variable> superclass;
  Class(lexer::Token name, std::vector<Var> fields,
        std::vector<Function> methods, std::optional<Variable> superclass,
        std::optional<std::vector<lexer::Token>> generics)
      : name(name), methods(methods), superclass(superclass),
        generics(generics) {}
};
template <typename T> struct SwitchCase {
  Expr value;
  T x;
  lexer::Token op;
  SwitchCase(Expr value, T x, lexer::Token op) : value(value), x(x), op(op) {}
};
template <typename T> class Switch : public Stmt {
public:
  Expr condition;
  std::vector<SwitchCase<T>> cases;
  std::optional<Stmt> defaultCase;
  Switch(Expr condition, std::vector<SwitchCase<T>> cases,
         std::optional<Stmt> defaultCase)
      : condition(condition), cases(cases), defaultCase(defaultCase) {}
};
} // namespace ktpp::parser