#include "Lexer/Lexer.hh"
#include "Stmt.hh"
#include <vector>

using namespace ktpp::lexer;

namespace ktpp::parser {
  class Parser {
  public:
    bool hadError = false;
    Parser(ktpp::logger::Logger* logger, std::string filePath, std::string source);
    std::vector<Stmt> parse();

  private:
    ktpp::logger::Logger* logger;
    std::string filePath;
    std::string source;
    std::vector<Token> tokens;

    Stmt declaration();
    Stmt statement();
    If ifStatement();
    Switch<Stmt> switchStatement();
    std::variant<For, ForEach> forStatement();
    While whileStatement(bool isDoWhile = false);
    Break breakStatement();
    Continue continueStatement();
    Class classDeclaration();
    Return returnStatement();
    Function functionDeclaration();
    Var varDeclaration();
    Expression expressionStatement();
    std::vector<Stmt> scope();

    std::optional<Expr> canExpression();
    Expr expression() throw(ParseError);

    Binary binary();
    Unary unary();
    Grouping grouping();
    Variable variable();
    Assign assign();
    Generic generic();
    Type type();
    Lambda lambda();

    void synchronize();
    void error(Token token, std::string message) throw(ParseError);

    Token peek(size_t offset = 0);
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
} // namespace ktpp::parser