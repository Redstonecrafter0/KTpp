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
  return (OtherKind &)kind == OtherKind::Question_Mark ||
         (OtherKind &)kind == OtherKind::Colon ||
         (OtherKind &)kind == OtherKind::Arrow;
}

bool isBinop(OperatorKind kind) {
  return !(isStrictlyUnop(kind) || isAssignment(kind) || isOperational(kind));
}

Parser::Parser(ktpp::logger::Logger *logger, std::string filePath,
               std::string source)
    : logger(logger), filePath(filePath), source(source) {
  Lexer lexer = Lexer(logger, filePath, source);
  lexer.lex();
  if (lexer.hadError)
    hadError = 1;
  else
    tokens = lexer.tokens;
}
std::vector<std::unique_ptr<Stmt>> Parser::parse() {
  std::vector<std::unique_ptr<Stmt>> statements{};
  if (!hadError) try {
      while (!isAtEnd()) statements.push_back(declaration());
    } catch (ParseError &e) {
      hadError = 0;
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
    return;
  }
}

std::unique_ptr<Stmt> Parser::statement() {
  if (match(KeywordKind::For))
    return std::get<std::unique_ptr<For>>(forStatement());
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

std::variant<For, ForEach> Parser::forStatement() {
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
        return ForEach(
            std::move((std::unique_ptr<Var> &)initializer.value()),
            consume(OtherKind::Colon, "Expect ':' after initialization."),
            std::move(expression()), std::move(statement()));

  std::optional<std::unique_ptr<Expr>> condition = std::nullopt;
  if (!check(OtherKind::Semicolon)) condition = expression();
  consume(OtherKind::Semicolon, "Expect ';' after loop condition.");

  std::optional<std::unique_ptr<Expr>> increment = std::nullopt;
  if (!check(OtherKind::R_Paren)) increment = expression();

  std::unique_ptr<Stmt> body = statement();
  if (increment.has_value())
    body = std::make_unique<Scope>(
        Scope({std::move(body), std::make_unique<Expression>(Expression(
                                    std::move(increment.value())))}));
  if (condition.has_value())
    condition = std::make_unique<Literal>(Literal(true));
  return For(std::move(initializer.value()), std::move(condition.value()),
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
  std::optional<std::vector<Generic>> generics = std::nullopt;
  if (match(OperatorKind::Less)) {
    generics = std::vector<Generic>();
    while (!match(OperatorKind::Greater)) generics.value().push_back(generic());
  }

  std::optional<std::unique_ptr<Expr>> superclass = std::nullopt;
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
  return Class(name, fields, methods, std::move(superclass), generics);
}

Return Parser::returnStatement() {
  Token kw = peek(-1);
  std::optional<std::unique_ptr<Expr>> v = canExpression();
  return Return(kw, v.has_value() ? std::make_optional(std::move(v.value()))
                                  : std::nullopt);
}

Function Parser::functionDeclaration() {
  Token name = consume(LiteralKind::Identifier, "Expect identifier.");

  std::optional<std::vector<Generic>> generics = std::nullopt;
  if (match(OperatorKind::Less)) {
    generics = std::vector<Generic>();
    while (!match(OperatorKind::Greater)) generics.value().push_back(generic());
  }

  std::optional<std::vector<Param>> params = std::nullopt;
  if (match(OtherKind::L_Paren)) {
    params = {};
    while (!match(OtherKind::R_Paren)) {
      consume(LiteralKind::Identifier, "Expect identifier.");
      if (match(OtherKind::Comma))
        if (peek(1).kind != (TokenKind)(OperatorKind::Greater) &&
            peek(1).kind != (TokenKind)(LiteralKind::Identifier))
          error(peek(), "Expect identifier.");
        else {
          Token n = peek(-2);
          consume(OtherKind::Colon, "Expect ':' after parameter name.");
          params.value().push_back(Param(n, std::make_unique<Type>(type())));
        }
    }
  }
  return Function(name, generics, params,
                  match(OtherKind::Arrow)
                      ? std::make_optional(std::make_unique<Type>(type()))
                      : std::nullopt,
                  Scope(scope()));
}

Var Parser::varDeclaration() {
  bool isMut = match(KeywordKind::Mut);
  Token name = consume(LiteralKind::Identifier, "Expect variable name.");
  std::optional<std::unique_ptr<Type>> t =
      match(OtherKind::Colon)
          ? std::make_optional(std::make_unique<Type>(type()))
          : std::nullopt;
  std::optional<std::unique_ptr<Expr>> initializer;

  if (match(OtherKind::Colon))
    if (match(OperatorKind::Equals))
      initializer = expression();
    else if (!isMut && !initializer.has_value())
      error(name, "'const' declarations must be initialized.");
  return Var(
      name,
      initializer.has_value()
          ? std::make_optional(std::move(initializer.value()))
          : std::nullopt,
      t.has_value() ? std::make_optional(std::move(t.value())) : std::nullopt,
      isMut);
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
  if (isBinop((OperatorKind &)t.kind))
    return checkExtension(
        std::make_unique<Binary>(binary(std::move(first), t)));
}

std::unique_ptr<Expr> Parser::expression() throw(ParseError) {
  Token t = peek();
  std::optional<std::unique_ptr<Expr>> expr = canExpression();
  if (!expr.has_value()) {
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
          isAssignment((OperatorKind &)peek().kind) ? (Expr)assign(t)
                                                    : (Expr)variable(t)));
    if (match({LiteralKind::Float, LiteralKind::Int, LiteralKind::String,
               KeywordKind::True, KeywordKind::False}))
      if (match({OperatorKind::Bang, OperatorKind::Minus, OperatorKind::Inv}))
        return checkExtension(std::make_unique<Literal>(literal(t)));
  } else {
    if (match(OtherKind::Dot)) {
      Token member = consume(LiteralKind::Identifier, "Expect member name.");
      return checkExtension(std::make_unique<Expr>(
          isAssignment((OperatorKind &)peek().kind)
              ? (Expr)set(std::move(expr.value()), member)
              : (Expr)get(std::move(expr.value()), member)));
    }
    if (isBinop((OperatorKind &)t.kind))
      return checkExtension(
          std::make_unique<Binary>(binary(expression(), advance())));
    if (match(OtherKind::Question_Mark))
      return checkExtension(
          std::make_unique<Ternary>(ternary(std::move(expr.value()), t)));
    if (match(OtherKind::L_Bracket))
      return checkExtension(std::make_unique<Array>(array(t)));
  }

  error(t, "Expect expression.");
}

Literal Parser::literal(Token t) {
  if ((KeywordKind &)t.kind == KeywordKind::False) return Literal(false);
  if ((KeywordKind &)t.kind == KeywordKind::True) return Literal(true);
  if ((LiteralKind &)t.kind == LiteralKind::String) return Literal(t.literal);
  if ((LiteralKind &)t.kind == LiteralKind::Int) return Literal(t.literal);
  if ((LiteralKind &)t.kind == LiteralKind::Float) return Literal(t.literal);
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

Call Parser::call() {}
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
  std::optional<std::vector<std::unique_ptr<Expr>>> values = std::nullopt;
  if (!match(OtherKind::R_Bracket))
    while (!match(OtherKind::R_Bracket)) {
      values.value().push_back(std::make_unique<Generic>(generic()));
      if (match(OtherKind::Comma)) continue;
    }

  return Array(bracket, values);
}
IndexSignature Parser::indexing(std::unique_ptr<Expr> array, Token bracket) {
  std::unique_ptr<Expr> e = expression();
  consume(OtherKind::R_Bracket, "Expect ']'.");
  return IndexSignature(std::move(array), std::move(e), bracket);
}

Grouping Parser::grouping() { return Grouping(expression()); }

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

  std::optional<std::unique_ptr<Type>> extends =
      match(OtherKind::Colon)
          ? std::make_optional(std::make_unique<Type>(type()))
          : std::nullopt;
  return Type(
      id, generics.size() > 0 ? std::make_optional(generics) : std::nullopt,
      extends.has_value() ? std::make_optional(std::move(extends.value()))
                          : std::nullopt);
}

Lambda Parser::lambda() {
  Token kw = peek(-1);
  std::optional<std::vector<Generic>> generics = std::nullopt;
  if (match(OperatorKind::Less)) {
    generics = std::vector<Generic>();
    while (!match(OperatorKind::Greater)) generics.value().push_back(generic());
  }

  std::optional<std::vector<Param>> params = std::nullopt;
  if (match(OtherKind::L_Paren)) {
    params = {};
    while (!match(OtherKind::R_Paren)) {
      consume(LiteralKind::Identifier, "Expect identifier.");
      if (match(OtherKind::Comma))
        if (peek(1).kind != (TokenKind)(OperatorKind::Greater) &&
            peek(1).kind != (TokenKind)(LiteralKind::Identifier))
          error(peek(), "Expect identifier.");
        else {
          Token n = peek(-2);
          consume(OtherKind::Colon, "Expect ':' after parameter name.");
          params.value().push_back(Param(n, std::make_unique<Type>(type())));
        }
    }
  } else
    params = std::nullopt;
  std::optional<std::unique_ptr<Type>> t =
      match(OtherKind::Arrow)
          ? std::make_optional(std::make_unique<Type>(type()))
          : std::nullopt;
  return Lambda(
      kw, generics, params,
      t.has_value() ? std::make_optional(std::move(t.value())) : std::nullopt,
      expression());
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