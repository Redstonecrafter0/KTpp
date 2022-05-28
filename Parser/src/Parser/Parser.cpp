#include "Parser/Parser.hh"

template <typename Base, typename T> inline bool instanceof (const T) {
  return std::is_base_of<Base, T>::value;
}

namespace ktpp::parser {
using OperatorKind = lexer::OperatorKind;
using KeywordKind = lexer::KeywordKind;
using OtherKind = lexer::OtherKind;
using LiteralKind = lexer::LiteralKind;

bool is_strictly_unop(OperatorKind kind) { return kind == OperatorKind::Bang; }

bool is_assignment(OperatorKind kind) {
  return kind == OperatorKind::Equals || kind == OperatorKind::PlusEq ||
         kind == OperatorKind::MinusEq || kind == OperatorKind::StarEq ||
         kind == OperatorKind::SlashEq || kind == OperatorKind::ModEq ||
         kind == OperatorKind::AndEq || kind == OperatorKind::OrEq ||
         kind == OperatorKind::PowerEq;
}

bool is_binop(OperatorKind kind) {
  return !(is_strictly_unop(kind) || is_assignment(kind));
}

Parser::Parser(logger::Logger *logger, std::vector<lexer::Token> tokens)
    : logger(logger), tokens(tokens) {
  tokens = tokens;
}
std::vector<parser::Stmt> Parser::parse() {
  std::vector<Stmt> statements{};
  try {
    while (!isAtEnd())
      statements.push_back(declaration());
  } catch (ParseError &e) {
    logger->Log(logger::LogLevel::Error, "Parser", e.what());
    hadError = 0;
    return std::vector<parser::Stmt>();
  }
  return statements;
}

Stmt Parser::declaration() {
  if (match(KeywordKind::Class))
    return classDeclaration();
  if (match(KeywordKind::Fn))
    return functionDeclaration();
  if (match(KeywordKind::Var))
    return varDeclaration();
  return statement();
}

Stmt Parser::statement() {
  if (match(KeywordKind::For))
    return forStatement();
  if (match(KeywordKind::If))
    return ifStatement();
  if (match(KeywordKind::Return))
    return returnStatement();
  if (match(KeywordKind::Do)) {
    consume(KeywordKind::While, "Expect \"while\" after \"do\".");
    return whileStatement(true);
  }
  if (match(KeywordKind::While))
    return whileStatement();
  if (match(KeywordKind::Break))
    return breakStatement();
  if (match(KeywordKind::Continue))
    return continueStatement();
  if (match(OtherKind::L_Brace))
    return Block({block()});
  return expressionStatement();
}

If Parser::ifStatement() {
  Expr condition = expression();

  Stmt thenBranch = statement();
  std::optional<ktpp::parser::Stmt> elseBranch;
  if (match(KeywordKind::Else))
    elseBranch = statement();
  return If(condition, thenBranch, elseBranch);
}

Switch<Stmt> Parser::switchStatement() {
  Expr condition = expression();

  consume(OtherKind::R_Brace, "Expect '{' after condition.");
  std::vector<SwitchCase<Stmt>> cases;
  std::optional<Stmt> defaultCase;
  while (!match(OtherKind::R_Brace)) {
    if (peek().lexeme == "_") {
      if (defaultCase)
        error(peek(), "Default case already exists.");
      advance();
      defaultCase = Block(block());
      continue;
    }
    Expr v = expression();
    consume(OtherKind::Arrow, "Expect \"->\" after value.");
    lexer::Token arrow = peek(-1);
    cases.push_back(SwitchCase<Stmt>(v, Block(block()), arrow));
  }

  return Switch<Stmt>(condition, cases, defaultCase);
}

Stmt Parser::forStatement() {
  std::optional<Stmt> initializer;
  if (match(OtherKind::Semicolon))
    initializer = std::nullopt;
  else if (match(KeywordKind::Var))
    initializer = varDeclaration();
  else
    initializer = expressionStatement();
  if (initializer != std::nullopt)
    consume(OtherKind::Semicolon, "Expect ';' after loop condition.");

  if (initializer.has_value())
    if (instanceof <Var>(initializer.value()))
      if (!((Var &)initializer.value()).initializer.has_value())
        return ForEach(
            (Var &)initializer.value(),
            consume(OtherKind::Colon, "Expect ':' after initialization."),
            expression(), statement());

  std::optional<Expr> condition = std::nullopt;
  if (!check(OtherKind::Semicolon))
    condition = expression();
  consume(OtherKind::Semicolon, "Expect ';' after loop condition.");

  std::optional<Expr> increment = std::nullopt;
  if (!check(OtherKind::R_Paren))
    increment = expression();

  Stmt body = statement();
  if (increment.has_value())
    body = Block({body, Expression(increment.value())});
  if (condition.has_value())
    condition = Literal(true);
  return For(initializer.value(), condition.value(), increment.value(), body);
}

While Parser::whileStatement(bool isDoWhile) {
  Expr condition = expression();
  Stmt body = statement();
  return While(condition, body, isDoWhile);
}

Break Parser::breakStatement() { return Break(peek(-1)); }

Continue Parser::continueStatement() { return Continue(peek(-1)); }

Class Parser::classDeclaration() {
  lexer::Token name = consume(LiteralKind::Identifier, "Expect class name.");
  std::optional<std::vector<lexer::Token>> generics{};
  if (match(OperatorKind::Less))
    while (!match(OperatorKind::Greater)) {
      consume(LiteralKind::Identifier, "Expect identifier.");
      if (match(OtherKind::Comma)) {
        if (peek(1).kind != (lexer::TokenKind)(OperatorKind::Greater) &&
            peek(1).kind != (lexer::TokenKind)(LiteralKind::Identifier)) {
          error(peek(), "Expect identifier.");
        } else {
          generics.value().push_back(peek(-2));
        }
      }
    }

  std::optional<Variable> superclass = std::nullopt;
  if (match(OtherKind::Colon)) {
    if (!check(LiteralKind::Identifier))
      error(peek(), "Expect identifier.");
    else
      p1();
  }

  consume(OtherKind::L_Brace, "Expect '{' before class body.");

  std::vector<Var> fields{};
  std::vector<Function> methods{};
  while (check(OtherKind::R_Brace) && !isAtEnd())
    if (match(KeywordKind::Fn))
      methods.push_back(functionDeclaration());
    else
      fields.push_back(varDeclaration());

  consume(OtherKind::R_Brace, "Expect '}' after class body.");
  return Class(name, fields, methods, superclass,
               generics.value().size() > 0 ? generics : std::nullopt);
}

Return Parser::returnStatement() {
  lexer::Token kw = peek(-1);
  std::optional<Expr> v = canExpression();
  return Return(kw, v);
}

Function Parser::functionDeclaration() {
  lexer::Token name = consume(LiteralKind::Identifier, "Expect identifier.");

  std::vector<lexer::Token> generics{};
  if (match(OperatorKind::Less))
    while (!match(OperatorKind::Greater)) {
      generics.push_back(advance());
      if (match(OtherKind::Comma))
        continue;
      if (!match(OperatorKind::Greater))
        error(peek(), "Expect ',' or '>'.");
    }

  std::optional<std::vector<Param>> params{};
  if (match(OtherKind::L_Paren))
    while (!match(OtherKind::R_Paren)) {
      consume(LiteralKind::Identifier, "Expect identifier.");
      if (match(OtherKind::Comma)) {
        if (peek(1).kind != (lexer::TokenKind)(OperatorKind::Greater) &&
            peek(1).kind != (lexer::TokenKind)(LiteralKind::Identifier)) {
          error(peek(), "Expect identifier.");
        } else {
          lexer::Token n = peek(-2);
          consume(OtherKind::Colon, "Expect ':' after parameter name.");
          params.value().push_back(Param(n, parseType()));
        }
      }
    }
  else
    params = std::nullopt;

  consume(OtherKind::Arrow, "Expect '->' after parameter list.");
  Type type = parseType();

  return Function(
      name, generics.size() > 0 ? std::make_optional(generics) : std::nullopt,
      params, Block(block()), type);
}

// Var Parser::varDeclaration();

Expression Parser::expressionStatement() {
  Expr expr = expression();
  consume(OtherKind::Semicolon, "Expect ';' after expression.");
  return Expression(expr);
}

std::vector<Stmt> Parser::block() {
  std::vector<Stmt> statements{};
  while (!check(OtherKind::R_Brace) && !isAtEnd())
    statements.push_back(declaration());

  consume(OtherKind::R_Brace, "Expect '}' after block.");
  return statements;
}

Type Parser::parseType() {
  lexer::Token id = consume(LiteralKind::Identifier, "Expect type name.");
  std::vector<Type> generics{};
  if (match(OperatorKind::Less))
    while (!match(OperatorKind::Greater)) {
      generics.push_back(parseType());
      if (match(OtherKind::Comma))
        continue;
      if (!match(OperatorKind::Greater))
        error(peek(), "Expect ',' or '>'.");
    }
  return Type(id, generics.size() > 0 ? std::make_optional(generics)
                                      : std::nullopt);
}

std::optional<Expr> Parser::canExpression() { return p14(); }
Expr Parser::expression() {
  lexer::Token t = peek();
  std::optional<Expr> expr = canExpression();
  if (!expr.has_value())
    error(t, "Expect expression.");
  return expr.value();
}

void Parser::error(lexer::Token token, std::string message) {
  throw ParseError("[" + std::to_string(token.line) + ":" +
                   std::to_string(token.position) + "]" + message);
}
void Parser::synchronize() {
  advance();
  while (!isAtEnd()) {
    if (std::holds_alternative<KeywordKind>(peek().kind))
      switch (std::get<KeywordKind>(peek().kind)) {
      case KeywordKind::Class:
      case KeywordKind::Fn:
      case KeywordKind::Var:
      case KeywordKind::For:
      case KeywordKind::If:
      case KeywordKind::Switch:
      case KeywordKind::While:
      case KeywordKind::Return:
        return;
      default:
        break;
      }

    advance();
  }
}

lexer::Token Parser::consume(lexer::TokenKind type, std::string message) {
  if (check(type))
    return advance();
  error(peek(), message);
}

bool Parser::isAtEnd() {
  return peek().kind == (lexer::TokenKind)OtherKind::Eof;
}
bool Parser::check(lexer::TokenKind kind) {
  return isAtEnd() ? false : peek().kind == kind;
}
bool Parser::match(lexer::TokenKind kind) {
  if (check(kind)) {
    advance();
    return true;
  }
  return false;
}
bool Parser::match(std::vector<lexer::TokenKind> kinds) {
  for (auto kind : kinds) {
    if (check(kind)) {
      advance();
      return true;
    }
  }
  return false;
}
} // namespace ktpp::parser