#include <vector>

#include "Lexer/Lexer.hh"
#include "Stmt.hh"

using namespace ktpp::lexer;

namespace ktpp::parser {
class Parser {
 public:
  bool hadError = false;
  Parser(ktpp::logger::Logger *logger, std::string filePath,
         std::vector<Token> source);
  std::vector<std::unique_ptr<Stmt>> parse();

 private:
  size_t current = 0;
  ktpp::logger::Logger *logger;
  std::string filePath;
  std::string source;
  std::vector<Token> tokens;

  std::unique_ptr<Stmt> declaration();
  std::unique_ptr<Stmt> statement();
  If ifStatement();
  Switch<std::unique_ptr<Stmt>> switchStatement();
  std::unique_ptr<Stmt> forStatement();
  While whileStatement(bool isDoWhile = false);
  Break breakStatement();
  Continue continueStatement();
  Class classDeclaration();
  Return returnStatement();
  Function functionDeclaration();
  Var varDeclaration();
  Expression expressionStatement();
  std::vector<std::unique_ptr<Stmt>> scope();

  std::optional<std::unique_ptr<Expr>> canExpression();
  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> checkExtension(std::unique_ptr<Expr> first);

  Literal literal(Token t);
  Binary binary(std::unique_ptr<Expr> left, Token op);
  Unary unary(Token op, std::unique_ptr<Expr> right);
  Grouping grouping();
  Variable variable(Token name);
  Assign assign(Token name);
  Call call();
  Get get(std::unique_ptr<Expr> object, Token member);
  Set set(std::unique_ptr<Expr> object, Token member);
  Ternary ternary(std::unique_ptr<Expr> condition, Token questionM);
  This thisExpr(Token keyword);
  Array array(Token bracket);
  IndexSignature indexing(std::unique_ptr<Expr> array, Token bracket);
  GenericParameters genericParams();
  GenericArgs genericArgs();
  Type type();
  Lambda lambda();

  void synchronize();
  void error(Token token, std::string message);

  Token peek(int64_t offset = 0);
  Token advance();
  Token consume(TokenKind type, std::string message);
  bool isAtEnd();
  bool check(TokenKind kind);
  bool match(TokenKind kind);
  bool match(std::vector<TokenKind> kinds);
};

struct ParseError : public std::exception {
 public:
  std::string message;
  ParseError(std::string message) : message(message) {}
};
}  // namespace ktpp::parser