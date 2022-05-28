#include "Lexer/Lexer.hh"
#include "Stmt.hh"
#include <vector>

namespace ktpp::parser {
class Parser {
public:
  bool hadError = false;
  Parser(logger::Logger *logger, std::vector<lexer::Token> tokens);
  std::vector<parser::Stmt> parse();

private:
  logger::Logger *logger;
  std::vector<lexer::Token> tokens;

  Stmt declaration();
  Stmt statement();
  If ifStatement();
  Switch<Stmt> switchStatement();
  Stmt forStatement();
  While whileStatement(bool isDoWhile = false);
  Break breakStatement();
  Continue continueStatement();
  Class classDeclaration();
  Return returnStatement();
  Function functionDeclaration();
  Var varDeclaration();
  Expression expressionStatement();
  std::vector<Stmt> block();
  Type parseType();

  std::optional<Expr> canExpression();
  Expr expression();
  std::optional<Expr> p14();
  std::optional<Expr> p13();
  std::optional<Expr> p12();
  std::optional<Expr> p11();
  std::optional<Expr> p10();
  std::optional<Expr> p9();
  std::optional<Expr> p8();
  std::optional<Expr> p7();
  std::optional<Expr> p6();
  std::optional<Expr> p5();
  std::optional<Expr> p4();
  std::optional<Expr> p3();
  std::optional<Expr> p2();
  std::optional<Expr> p1();

  void synchronize();
  void error(lexer::Token token, std::string message);

  lexer::Token peek(size_t offset = 0);
  lexer::Token advance();
  lexer::Token consume(lexer::TokenKind type, std::string message);
  bool isAtEnd();
  bool check(lexer::TokenKind kind);
  bool match(lexer::TokenKind kind);
  bool match(std::vector<lexer::TokenKind> kinds);
};

struct ParseError : public std::exception {
public:
  std::string message;
  ParseError(std::string message) : message(message) {}
};
} // namespace ktpp::parser