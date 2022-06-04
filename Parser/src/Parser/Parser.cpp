#include "Parser.hh"

template <typename Base, typename T>
inline bool instanceof (const T) {
  return std::is_base_of<Base, T>::value;
}

namespace ktpp::parser {

template <typename T>
std::pair<bool, T> isTokenKind(const TokenKind &token) {
  if (std::holds_alternative<T>(token)) {
    return std::make_pair(true, std::get<T>(token));
  }
  return std::make_pair(false, T());
}

bool isStrictlyUnop(TokenKind kind) {
  auto [isUnop, unop] = isTokenKind<OperatorKind>(kind);
  return isUnop && (unop == OperatorKind::Bang || unop == OperatorKind::Inv);
}

bool isAssignment(TokenKind kind) {
  auto [isAssign, assign] = isTokenKind<OperatorKind>(kind);
  return isAssign &&
         (assign == OperatorKind::Equals || assign == OperatorKind::PlusEq ||
          assign == OperatorKind::MinusEq || assign == OperatorKind::StarEq ||
          assign == OperatorKind::SlashEq || assign == OperatorKind::ModEq ||
          assign == OperatorKind::AndEq || assign == OperatorKind::OrEq ||
          assign == OperatorKind::PowerEq);
}

bool isOperational(TokenKind kind) {
  auto [isOp, op] = isTokenKind<OtherKind>(kind);
  return isOp && (op == OtherKind::Question_Mark || op == OtherKind::Colon ||
                  op == OtherKind::Arrow);
}

bool isEof(TokenKind kind) {
  auto [isEof, eof] = isTokenKind<OtherKind>(kind);
  return isEof && eof == OtherKind::Eof;
}

bool isBinop(TokenKind kind) {
  return !(isEof(kind) || isStrictlyUnop(kind) || isAssignment(kind) ||
           isOperational(kind));
}

Parser::Parser(ktpp::logger::Logger *logger, std::string filePath,
               std::vector<Token> source)
    : logger(logger), filePath(filePath), tokens(source) {}
std::vector<std::unique_ptr<Stmt>> Parser::parse() {
  std::vector<std::unique_ptr<Stmt>> statements{};
  if (!hadError) try {
      while (!isAtEnd()) {
        auto decl = declaration();
        statements.push_back(std::move(decl));
      }
    } catch (ParseError &e) {
      hadError = true;
      return std::vector<std::unique_ptr<Stmt>>();
    }
  return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
  try {
    if (match(KeywordKind::Class))
      return std::make_unique<Class>(classDeclaration());
    if (match(KeywordKind::Fn))
      if (match(LiteralKind::Identifier))
        return std::make_unique<Function>(functionDeclaration());
    if (match(KeywordKind::Var)) return std::make_unique<Var>(varDeclaration());
    return statement();
  } catch (std::any &) {
    synchronize();
    return declaration();
  }
}

std::unique_ptr<Stmt> Parser::statement() {
  if (match(KeywordKind::For)) return forStatement();
  if (match(KeywordKind::If)) return std::make_unique<If>(ifStatement());
  if (match(KeywordKind::Return))
    return std::make_unique<Return>(returnStatement());
  if (match(KeywordKind::Do)) {
    consume(KeywordKind::While, "Expect \"while\" after \"do\".");
    return std::make_unique<While>(whileStatement(true));
  }
  if (match(KeywordKind::While))
    return std::make_unique<While>(whileStatement());
  if (match(KeywordKind::Break))
    return std::make_unique<Break>(breakStatement());
  if (match(KeywordKind::Continue))
    return std::make_unique<Continue>(continueStatement());
  if (match(OtherKind::L_Brace))
    return std::make_unique<Scope>(Scope({scope()}));
  return std::make_unique<Expression>(expressionStatement());
}

If Parser::ifStatement() {
  std::unique_ptr<Expr> condition = expression();

  std::unique_ptr<Stmt> thenBranch = statement();
  std::optional<std::unique_ptr<Stmt>> elseBranch;
  if (match(KeywordKind::Else)) elseBranch = statement();
  return If(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

Switch<std::unique_ptr<Stmt>> Parser::switchStatement() {
  std::unique_ptr<Expr> condition = expression();

  consume(OtherKind::R_Brace, "Expect '{' after condition.");
  std::vector<SwitchCase<std::unique_ptr<Stmt>>> cases;
  std::optional<std::unique_ptr<Stmt>> defaultCase;
  while (!match(OtherKind::R_Brace)) {
    if (peek().lexeme == "_") {
      if (defaultCase) error(peek(), "Default case already exists.");
      advance();
      defaultCase = std::make_unique<Scope>(Scope(scope()));
      continue;
    }
    std::unique_ptr<Expr> v = expression();
    consume(OtherKind::Arrow, "Expect \"->\" after value.");
    Token arrow = peek(-1);
    cases.push_back(SwitchCase<std::unique_ptr<Stmt>>(
        std::move(v), std::make_unique<Scope>(Scope(scope())), arrow));
  }

  return Switch<std::unique_ptr<Stmt>>(
      std::move(condition), std::move(cases),
      defaultCase.has_value()
          ? std::make_optional(std::move(defaultCase.value()))
          : std::nullopt);
}

std::unique_ptr<Stmt> Parser::forStatement() {
  std::optional<std::unique_ptr<Stmt>> initializer;
  if (match(OtherKind::Semicolon))
    initializer = std::nullopt;
  else if (match(KeywordKind::Var))
    initializer = std::make_unique<Var>(varDeclaration());
  else
    initializer = std::make_unique<Expression>(expressionStatement());
  if (initializer != std::nullopt)
    consume(OtherKind::Semicolon, "Expect ';' after loop condition.");

  if (initializer.has_value())
    if (instanceof <Var>(std::move(initializer.value())))
      if (!((Var &)initializer.value()).initializer.has_value())
        return std::make_unique<ForEach>(
            std::move((std::unique_ptr<Var> &)initializer.value()),
            consume(OtherKind::Colon, "Expect ':' after initialization."),
            std::move(expression()), std::move(statement()));

  std::optional<std::unique_ptr<Expr>> condition = std::nullopt;
  if (!check(OtherKind::Semicolon)) condition = expression();
  consume(OtherKind::Semicolon, "Expect ';' after loop condition.");

  std::optional<std::unique_ptr<Expr>> increment = std::nullopt;
  if (!check(OtherKind::R_Paren)) increment = expression();

  std::unique_ptr<Stmt> body = statement();
  if (condition.has_value())
    condition = std::make_unique<Literal>(Literal(true));
  return std::make_unique<For>(std::move(initializer.value()),
                               std::move(condition.value()),
                               std::move(increment.value()), std::move(body));
}

While Parser::whileStatement(bool isDoWhile) {
  std::optional<std::unique_ptr<Expr>> condition = canExpression();
  std::unique_ptr<Stmt> body = statement();
  return While(condition.has_value() ? std::move(condition.value())
                                     : std::make_unique<Literal>(Literal(true)),
               std::move(body), isDoWhile);
}

Break Parser::breakStatement() { return Break(peek(-1)); }

Continue Parser::continueStatement() { return Continue(peek(-1)); }

Class Parser::classDeclaration() {
  Token name = consume(LiteralKind::Identifier, "Expect class name.");
  std::unique_ptr<GenericParameters> generics = nullptr;
  if (match(OperatorKind::Less)) {
    auto params = std::make_unique<GenericParameters>(genericParams());
    generics.swap(params);
  }

  std::unique_ptr<Expr> superclass = nullptr;
  if (match(OtherKind::Colon))
    if (!check(LiteralKind::Identifier))
      error(peek(), "Expect identifier.");
    else
      superclass = expression();

  consume(OtherKind::L_Brace, "Expect '{' before class body.");

  std::vector<std::unique_ptr<Var>> fields{};
  std::vector<Function> methods{};
  while (check(OtherKind::R_Brace) && !isAtEnd())
    if (match(KeywordKind::Fn))
      methods.push_back(functionDeclaration());
    else
      fields.push_back(std::make_unique<Var>(varDeclaration()));

  consume(OtherKind::R_Brace, "Expect '}' after class body.");
  return Class(name, std::move(fields), std::move(methods),
               std::move(superclass), std::move(generics));
}

Return Parser::returnStatement() {
  Token kw = peek(-1);
  std::optional<std::unique_ptr<Expr>> v = canExpression();
  return Return(kw, v.has_value() ? std::make_optional(std::move(v.value()))
                                  : std::nullopt);
}

Function Parser::functionDeclaration() {
  Token name = consume(LiteralKind::Identifier, "Expect identifier.");

  std::unique_ptr<GenericParameters> generics = nullptr;
  if (match(OperatorKind::Less)) {
    auto params = std::make_unique<GenericParameters>(genericParams());
    generics.swap(params);
  }

  std::vector<Param> params;
  if (match(OtherKind::L_Paren)) {
    while (!match(OtherKind::R_Paren)) {
      consume(LiteralKind::Identifier, "Expect identifier.");
      if (match(OtherKind::Comma))
        if (peek(1).kind != (TokenKind)(OperatorKind::Greater) &&
            peek(1).kind != (TokenKind)(LiteralKind::Identifier))
          error(peek(), "Expect identifier.");
        else {
          Token n = peek(-2);
          consume(OtherKind::Colon, "Expect ':' after parameter name.");
          params.push_back(std::move(Param(n, std::make_unique<Type>(type()))));
        }
    }
  }
  return Function(
      name, std::move(generics), std::move(params),
      match(OtherKind::Arrow) ? std::make_optional(type()) : std::nullopt,
      std::move(statement()));
}

Var Parser::varDeclaration() {
  bool isMut = match(KeywordKind::Mut);
  Token name = consume(LiteralKind::Identifier, "Expect variable name.");
  std::optional<std::unique_ptr<Type>> t =
      match(OtherKind::Colon)
          ? std::make_optional(std::make_unique<Type>(type()))
          : std::nullopt;
  std::optional<std::unique_ptr<Expr>> initializer;

  if (match(OperatorKind::Equals))
    initializer = expression();
  else if (!isMut && !initializer.has_value())
    error(name, "'const' declarations must be initialized.");
  return Var(name, std::move(initializer), std::move(t), isMut);
}

Expression Parser::expressionStatement() { return Expression(expression()); }

std::vector<std::unique_ptr<Stmt>> Parser::scope() {
  std::vector<std::unique_ptr<Stmt>> statements{};
  while (!check(OtherKind::R_Brace) && !isAtEnd())
    statements.push_back(declaration());

  consume(OtherKind::R_Brace, "Expect '}' after block.");
  return statements;
}

std::optional<std::unique_ptr<Expr>> Parser::canExpression() {
  try {
    return expression();
  } catch (ParseError &) {
    return std::nullopt;
  }
}

std::unique_ptr<Expr> Parser::checkExtension(std::unique_ptr<Expr> first) {
  Token t = peek();
  if (isBinop(t.kind))
    return checkExtension(
        std::make_unique<Binary>(binary(std::move(first), t)));
  if (match(OtherKind::Dot)) {
    Token member = consume(LiteralKind::Identifier, "Expect member name.");
    return checkExtension(
        isAssignment(peek().kind)
            ? std::make_unique<Expr>(set(std::move(first), member))
            : std::make_unique<Expr>(get(std::move(first), member)));
  }
  if (isBinop(t.kind))
    return checkExtension(
        std::make_unique<Binary>(binary(expression(), advance())));
  if (match(OtherKind::Question_Mark))
    return checkExtension(
        std::make_unique<Ternary>(ternary(std::move(first), t)));
  if (match(OtherKind::L_Bracket))
    return checkExtension(std::make_unique<Array>(array(t)));
  return first;
}

std::unique_ptr<Expr> Parser::expression() {
  Token t = peek();
  if (match(KeywordKind::Fn))
    return checkExtension(std::make_unique<Lambda>(lambda()));
  if (match(KeywordKind::This))
    return checkExtension(std::make_unique<This>(thisExpr(t)));
  if (match(OtherKind::L_Paren))
    return checkExtension(std::make_unique<Grouping>(grouping()));
  if (match(OtherKind::L_Bracket))
    return checkExtension(std::make_unique<Array>(array(t)));
  if (match(LiteralKind::Identifier))
    return checkExtension(std::make_unique<Expr>(
        isAssignment(peek().kind) ? (Expr)assign(t) : (Expr)variable(t)));
  if (match({LiteralKind::Float, LiteralKind::Int, LiteralKind::String,
             KeywordKind::True, KeywordKind::False}))
    return checkExtension(std::make_unique<Literal>(literal(t)));

  error(t, "Expect expression.");
}

Literal Parser::literal(Token t) {
  auto [isLiteral, kind] = isTokenKind<LiteralKind>(t.kind);
  auto [isKeyword, keyword] = isTokenKind<KeywordKind>(t.kind);
  if (isKeyword) {
    if (keyword == KeywordKind::False) return Literal(false);
    if (keyword == KeywordKind::True) return Literal(true);
  } else if (isLiteral) {
    if (kind == LiteralKind::String) return Literal(t.literal);
    if (kind == LiteralKind::Int) return Literal(t.literal);
    if (kind == LiteralKind::Float) return Literal(t.literal);
  }
  error(t, "Expect literal.");
}

Binary Parser::binary(std::unique_ptr<Expr> left, Token op) {
  return Binary(std::move(left), op, expression());
}

Unary Parser::unary(Token op, std::unique_ptr<Expr> right) {
  return Unary(std::move(right), op);
}

Variable Parser::variable(Token name) { return Variable(name); }

Assign Parser::assign(Token name) {
  Token t = peek(-1);
  std::unique_ptr<Expr> v = expression();
  std::optional<OperatorKind> k = std::nullopt;
  switch ((OperatorKind &)t.kind) {
    case OperatorKind::PlusEq:
      k = OperatorKind::Plus;
      break;
    case OperatorKind::MinusEq:
      k = OperatorKind::Minus;
      break;
    case OperatorKind::StarEq:
      k = OperatorKind::Star;
      break;
    case OperatorKind::SlashEq:
      k = OperatorKind::Slash;
      break;
    case OperatorKind::ModEq:
      k = OperatorKind::Mod;
      break;
    case OperatorKind::AndEq:
      k = OperatorKind::And;
      break;
    case OperatorKind::OrEq:
      k = OperatorKind::Or;
      break;
    case OperatorKind::PowerEq:
      k = OperatorKind::Power;
      break;
  }
  return Assign(name, k.has_value()
                          ? std::make_unique<Binary>(Binary(
                                std::make_unique<Variable>(Variable(name)),
                                Token(k.value(), "", t.span), std::move(v)))
                          : std::move(v));
}

Get Parser::get(std::unique_ptr<Expr> object, Token member) {
  return Get(std::move(object), member);
}
Set Parser::set(std::unique_ptr<Expr> object, Token member) {
  return Set(std::move(object), member, expression());
}
Ternary Parser::ternary(std::unique_ptr<Expr> condition, Token questionM) {
  return Ternary(std::move(condition), questionM, expression(),
                 consume(OtherKind::Colon, "Expect ':'."), expression());
}
This Parser::thisExpr(Token kw) { return This(kw); }
Array Parser::array(Token bracket) {
  std::vector<std::unique_ptr<Expr>> values;
  if (!match(OtherKind::R_Bracket))
    while (!match(OtherKind::R_Bracket)) {
      values.push_back(std::move(expression()));
      if (match(OtherKind::Comma)) continue;
    }

  return Array(bracket, std::move(values));
}
IndexSignature Parser::indexing(std::unique_ptr<Expr> array, Token bracket) {
  std::unique_ptr<Expr> e = expression();
  consume(OtherKind::R_Bracket, "Expect ']'.");
  return IndexSignature(std::move(array), std::move(e), bracket);
}

Grouping Parser::grouping() { return Grouping(expression()); }

GenericParameters Parser::genericParams() {
  Token LOp = peek(-1);
  std::vector<GenericParameter> params;

  while (!match(OperatorKind::Greater)) {
    auto param = GenericParameter(
        consume(LiteralKind::Identifier, "Expect identifier."),
        match(OtherKind::Colon) ? std::make_optional(type()) : std::nullopt);
    params.push_back(std::move(param));
    if (match(OtherKind::Comma))
      continue;
    else if (!match(OperatorKind::Greater))
      error(peek(), "Expect ',' or '>'.");
  }
  return GenericParameters(LOp, std::move(params));
}

Type Parser::type() {
  Token id = consume(LiteralKind::Identifier, "Expect type name.");
  std::unique_ptr<GenericParameters> generics =
      std::make_unique<GenericParameters>(id);
  if (match(OperatorKind::Less)) {
    generics = std::make_unique<GenericParameters>(genericParams());
  }

  std::optional<std::unique_ptr<Type>> extends =
      match(OtherKind::Colon)
          ? std::make_optional(std::make_unique<Type>(type()))
          : std::nullopt;
  return Type(id, std::move(generics), std::move(extends));
}

Lambda Parser::lambda() {
  Token kw = peek(-1);
  std::unique_ptr<GenericParameters> generics = nullptr;
  if (match(OperatorKind::Less)) {
    auto params = std::make_unique<GenericParameters>(genericParams());
    generics.swap(params);
  }

  std::vector<Param> params;
  if (match(OtherKind::L_Paren)) {
    while (!match(OtherKind::R_Paren)) {
      consume(LiteralKind::Identifier, "Expect identifier.");
      if (match(OtherKind::Comma))
        if (peek(1).kind != (TokenKind)(OperatorKind::Greater) &&
            peek(1).kind != (TokenKind)(LiteralKind::Identifier))
          error(peek(), "Expect identifier.");
        else {
          Token n = peek(-2);
          consume(OtherKind::Colon, "Expect ':' after parameter name.");
          params.push_back(Param(n, std::make_unique<Type>(type())));
        }
    }
  }
  std::optional<std::unique_ptr<Type>> t =
      match(OtherKind::Arrow)
          ? std::make_optional(std::make_unique<Type>(type()))
          : std::nullopt;
  return Lambda(kw, std::move(generics), std::move(params), std::move(t),
                expression());
}

void Parser::error(Token token, std::string message) {
  throw ParseError(message);
}
void Parser::synchronize() {
  advance();
  while (!isAtEnd()) {
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

Token Parser::advance() {
  if (!isAtEnd()) current++;
  return tokens[current - 1];
}

Token Parser::peek(int64_t offset) {
  return tokens[(size_t)((uint64_t)current + offset)];
}
}  // namespace ktpp::parser