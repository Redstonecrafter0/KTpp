#include "Parser.hh"

template <typename Base, typename T>
inline bool instanceof (const T) {
  return std::is_base_of<Base, T>::value;
}

namespace ktpp::parser {
bool isStrictlyUnop(OperatorKind kind) { return kind == OperatorKind::Bang; }

bool isAssignment(OperatorKind kind) {
  return kind == OperatorKind::Equals || kind == OperatorKind::PlusEq ||
         kind == OperatorKind::MinusEq || kind == OperatorKind::StarEq ||
         kind == OperatorKind::SlashEq || kind == OperatorKind::ModEq ||
         kind == OperatorKind::AndEq || kind == OperatorKind::OrEq ||
         kind == OperatorKind::PowerEq;
}

bool isOperational(OperatorKind kind) {
  return (OtherKind&)kind == OtherKind::Question_Mark ||
         (OtherKind&)kind == OtherKind::Colon ||
         (OtherKind&)kind == OtherKind::Arrow;
}

bool isBinop(OperatorKind kind) {
  return !(isStrictlyUnop(kind) || isAssignment(kind) || isOperational(kind));
}

Parser::Parser(ktpp::logger::Logger* logger, std::string filePath,
               std::string source)
    : logger(logger), filePath(filePath), source(source) {
  Lexer lexer = Lexer(logger, filePath, source);
  lexer.lex();
  tokens = lexer.tokens;
}
std::vector<parser::Stmt> Parser::parse() {
  std::vector<Stmt> statements{};
  try {
    while (!isAtEnd()) statements.push_back(declaration());
  } catch (ParseError& e) {
    hadError = 0;
    return std::vector<parser::Stmt>();
  }
  return statements;
}

Stmt Parser::declaration() {
  try {
    if (match(KeywordKind::Class)) return classDeclaration();
    if (match(KeywordKind::Fn))
      if (match(LiteralKind::Identifier)) return functionDeclaration();
    if (match(KeywordKind::Var)) return varDeclaration();
    return statement();
  } catch (std::any&) {
    synchronize();
    return;
  }
}

Stmt Parser::statement() {
  if (match(KeywordKind::For)) return std::get<Stmt>(forStatement());
  if (match(KeywordKind::If)) return ifStatement();
  if (match(KeywordKind::Return)) return returnStatement();
  if (match(KeywordKind::Do)) {
    consume(KeywordKind::While, "Expect \"while\" after \"do\".");
    return whileStatement(true);
  }
  if (match(KeywordKind::While)) return whileStatement();
  if (match(KeywordKind::Break)) return breakStatement();
  if (match(KeywordKind::Continue)) return continueStatement();
  if (match(OtherKind::L_Brace)) return Scope({scope()});
  return expressionStatement();
}

If Parser::ifStatement() {
  Expr condition = expression();

  Stmt thenBranch = statement();
  std::optional<ktpp::parser::Stmt> elseBranch;
  if (match(KeywordKind::Else)) elseBranch = statement();
  return If(condition, thenBranch, elseBranch);
}

Switch<Stmt> Parser::switchStatement() {
  Expr condition = expression();

  consume(OtherKind::R_Brace, "Expect '{' after condition.");
  std::vector<SwitchCase<Stmt>> cases;
  std::optional<Stmt> defaultCase;
  while (!match(OtherKind::R_Brace)) {
    if (peek().lexeme == "_") {
      if (defaultCase) error(peek(), "Default case already exists.");
      advance();
      defaultCase = Scope(scope());
      continue;
    }
    Expr v = expression();
    consume(OtherKind::Arrow, "Expect \"->\" after value.");
    Token arrow = peek(-1);
    cases.push_back(SwitchCase<Stmt>(v, Scope(scope()), arrow));
  }

  return Switch<Stmt>(condition, cases, defaultCase);
}

std::variant<For, ForEach> Parser::forStatement() {
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
      if (!((Var&)initializer.value()).initializer.has_value())
        return ForEach(
            (Var&)initializer.value(),
            consume(OtherKind::Colon, "Expect ':' after initialization."),
            expression(), statement());

  std::optional<Expr> condition = std::nullopt;
  if (!check(OtherKind::Semicolon)) condition = expression();
  consume(OtherKind::Semicolon, "Expect ';' after loop condition.");

  std::optional<Expr> increment = std::nullopt;
  if (!check(OtherKind::R_Paren)) increment = expression();

  Stmt body = statement();
  if (increment.has_value())
    body = Scope({body, Expression(increment.value())});
  if (condition.has_value()) condition = Literal(true);
  return For(initializer.value(), condition.value(), increment.value(), body);
}

While Parser::whileStatement(bool isDoWhile) {
  std::optional<Expr> condition = canExpression();
  Stmt body = statement();
  return While(condition.has_value() ? condition.value() : Literal(true), body,
               isDoWhile);
}

Break Parser::breakStatement() { return Break(peek(-1)); }

Continue Parser::continueStatement() { return Continue(peek(-1)); }

Class Parser::classDeclaration() {
  Token name = consume(LiteralKind::Identifier, "Expect class name.");
  std::optional<std::vector<Generic>> generics = std::nullopt;
  if (match(OperatorKind::Less)) {
    generics = std::vector<Generic>();
    while (!match(OperatorKind::Greater)) generics.value().push_back(generic());
  }

  std::optional<Expr> superclass = std::nullopt;
  if (match(OtherKind::Colon))
    if (!check(LiteralKind::Identifier))
      error(peek(), "Expect identifier.");
    else
      superclass = expression();

  consume(OtherKind::L_Brace, "Expect '{' before class body.");

  std::vector<Var> fields{};
  std::vector<Function> methods{};
  while (check(OtherKind::R_Brace) && !isAtEnd())
    if (match(KeywordKind::Fn))
      methods.push_back(functionDeclaration());
    else
      fields.push_back(varDeclaration());

  consume(OtherKind::R_Brace, "Expect '}' after class body.");
  return Class(name, fields, methods, superclass, generics);
}

Return Parser::returnStatement() {
  Token kw = peek(-1);
  std::optional<Expr> v = canExpression();
  return Return(kw, v);
}

Function Parser::functionDeclaration() {
  Token name = consume(LiteralKind::Identifier, "Expect identifier.");

  std::optional<std::vector<Generic>> generics = std::nullopt;
  if (match(OperatorKind::Less)) {
    generics = std::vector<Generic>();
    while (!match(OperatorKind::Greater)) generics.value().push_back(generic());
  }

  std::optional<std::vector<Param>> params{};
  if (match(OtherKind::L_Paren))
    while (!match(OtherKind::R_Paren)) {
      consume(LiteralKind::Identifier, "Expect identifier.");
      if (match(OtherKind::Comma))
        if (peek(1).kind != (TokenKind)(OperatorKind::Greater) &&
            peek(1).kind != (TokenKind)(LiteralKind::Identifier))
          error(peek(), "Expect identifier.");
        else {
          Token n = peek(-2);
          consume(OtherKind::Colon, "Expect ':' after parameter name.");
          params.value().push_back(Param(n, type()));
        }
    }
  else
    params = std::nullopt;
  return Function(
      name, generics, params, Scope(scope()),
      match(OtherKind::Arrow) ? std::make_optional(type()) : std::nullopt);
}

Var Parser::varDeclaration() {
  bool isMut = match(KeywordKind::Mut);
  Token name = consume(LiteralKind::Identifier, "Expect variable name.");
  std::optional<Type> t =
      match(OtherKind::Colon) ? std::make_optional(type()) : std::nullopt;
  std::optional<Expr> initializer;

  if (match(OtherKind::Colon))
    if (match(OperatorKind::Equals))
      initializer = expression();
    else if (!isMut && !initializer.has_value())
      error(name, "'const' declarations must be initialized.");
  return Var(name, initializer, t, isMut);
}

Expression Parser::expressionStatement() {
  Expr expr = expression();
  return Expression(expr);
}

std::vector<Stmt> Parser::scope() {
  std::vector<Stmt> statements{};
  while (!check(OtherKind::R_Brace) && !isAtEnd())
    statements.push_back(declaration());

  consume(OtherKind::R_Brace, "Expect '}' after block.");
  return statements;
}

std::optional<Expr> Parser::canExpression() {
  try {
    return expression();
  } catch (ParseError&) {
    return std::nullopt;
  }
}
Expr Parser::expression() throw(ParseError) {
  Token t = peek();
  std::optional<Expr> expr;
  if (match(KeywordKind::Fn)) return lambda();

  error(t, "Expect expression.");
  return expr.value();
}

Generic Parser::generic() {
  Token LOp = peek(-1);
  Generic generic = Generic(LOp, {});

  while (!match(OperatorKind::Greater)) {
    generic.type.insert(std::pair(
        consume(LiteralKind::Identifier, "Expect identifier."),
        match(OtherKind::Colon) ? std::make_optional(type()) : std::nullopt));
    if (match(OtherKind::Comma))
      continue;
    else if (!match(OperatorKind::Greater))
      error(peek(), "Expect ',' or '>'.");
  }
}

Type Parser::type() {
  Token id = consume(LiteralKind::Identifier, "Expect type name.");
  std::vector<Generic> generics{};
  if (match(OperatorKind::Less)) generics.push_back(generic());

  std::optional<Type> extends =
      match(OtherKind::Colon) ? std::make_optional(type()) : std::nullopt;
  return Type(id,
              generics.size() > 0 ? std::make_optional(generics) : std::nullopt,
              extends);
}

Lambda Parser::lambda() {
  Token kw = peek(-1);
  std::optional<std::vector<Generic>> generics = std::nullopt;
  if (match(OperatorKind::Less)) {
    generics = std::vector<Generic>();
    while (!match(OperatorKind::Greater)) generics.value().push_back(generic());
  }

  std::optional<std::vector<Param>> params{};
  if (match(OtherKind::L_Paren))
    while (!match(OtherKind::R_Paren)) {
      consume(LiteralKind::Identifier, "Expect identifier.");
      if (match(OtherKind::Comma))
        if (peek(1).kind != (TokenKind)(OperatorKind::Greater) &&
            peek(1).kind != (TokenKind)(LiteralKind::Identifier))
          error(peek(), "Expect identifier.");
        else {
          Token n = peek(-2);
          consume(OtherKind::Colon, "Expect ':' after parameter name.");
          params.value().push_back(Param(n, type()));
        }
    }
  else
    params = std::nullopt;
  std::optional<Type> t =
      match(OtherKind::Arrow) ? std::make_optional(type()) : std::nullopt;
  return Lambda(kw, generics, params, t, expression());
}

void Parser::error(Token token, std::string message) throw(ParseError) {
  throw ParseError(message);
}
void Parser::synchronize() {
  advance();
  while (!isAtEnd()) {
    switch (std::get<TokenKind>(peek().kind)) {
      case KeywordKind::Class:
      case KeywordKind::Fn:
      case KeywordKind::Var:
      case KeywordKind::For:
      case KeywordKind::If:
      case KeywordKind::Switch:
      case KeywordKind::While:
      case KeywordKind::Return:
        return;
    }

    advance();
  }
}

Token Parser::consume(TokenKind type, std::string message) {
  if (check(type)) return advance();
  error(peek(), message);
}
bool Parser::isAtEnd() { return peek().kind == (TokenKind)OtherKind::Eof; }
bool Parser::check(TokenKind kind) {
  return isAtEnd() ? false : peek().kind == kind;
}
bool Parser::match(TokenKind kind) {
  if (check(kind)) {
    advance();
    return true;
  }
  return false;
}
bool Parser::match(std::vector<TokenKind> kinds) {
  for (auto kind : kinds)
    if (check(kind)) {
      advance();
      return true;
    }
  return false;
}
}  // namespace ktpp::parser