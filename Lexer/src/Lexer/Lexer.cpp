#include "Lexer.hh"

#include <iostream>

namespace ktpp::lexer {
Lexer::Lexer(std::string source) : source(source) {}
void Lexer::lex() {
  while (!isAtEnd()) {
    start = current;
    try {
      tokenize();
    } catch (std::string e) {
      std::cout << "\033[1m\033[31m" << e << "\033[0m" << std::endl;
      hadError = true;
      break;
    }
  }

  tokens.push_back(Token(OtherKind::Eof, "", line, current, nullptr));
}

void Lexer::tokenize() {
  TokenKind kind = OtherKind::Bad;
  std::any literal = nullptr;

  auto other = otherToken();
  if (other.has_value()) {
    tokens.push_back(other.value());
    return;
  }
  char c = peek();

  if (iswspace(c)) {
    advance();
    return;
  }

  if (isalpha(c) || c == '_') {
    tokens.push_back(identifier());
    return;
  } else if (isdigit(c)) {
    tokens.push_back(number());
    return;
  }
  throw "Unexpected character '" + std::to_string(c) + "'.";
}

Token Lexer::number() {
  int64_t int_val;
  std::string lexme;
  double_t float_val, fraction_size;
  fraction_size = 1;
  bool is_float = false;
  bool is_power = false;
  int i = 0;

  for (;;) {
    int n = 0;
    char current = peek();
    if (current == '\0') break;
    if (current >= '0' && current <= '9')
      n = current - '0';
    else if (current == '_')
      continue;
    else if (current == '.') {
      if (is_float) throw "Invalid float literal";
      is_float = true;
      float_val = (double_t)int_val;
      continue;
    } else if (current == 'e') {
      is_power = true;
      lexme += 'e';
      advance();
      break;
    } else
      throw "Invalid number literal";
    if (is_float) {
      fraction_size /= 10;
      float_val += ((double_t)n) * (fraction_size);
    } else {
      int_val *= 10;
      int_val += n;
    }
    lexme += current;
    advance();
  }

  if (is_power) {
    int64_t power;
    for (;;) {
      char current = peek();
      if (!isdigit(current)) break;
      lexme += current;
      advance();
      power *= 10;
      power += current - '0';
    }

    if (power > 0) {
      if (!is_float) int_val = std::pow(int_val, power);
    } else if (!is_float) {
      float_val = (double_t)int_val;
      is_float ^= 1;
    }
    float_val = std::pow(float_val, (double_t)power);
  }

  if (is_float)
    return Token(LiteralKind::Float, lexme, position, line, float_val);
  else
    return Token(LiteralKind::Int, lexme, position, line, int_val);
}

Token Lexer::string() {
  while ((peek() != '"' || peek() != '\'') && !isAtEnd()) {
    if (peek() == '\n') line++;
    advance();
  }

  if (isAtEnd()) throw "Unterminated string.";
  advance();

  std::string value = source.substr(start + 1, current - start - 1);
  return Token(LiteralKind::String, source.substr(start, current - start),
               position, line, value);
}

Token Lexer::identifier() {
  while (isalnum(peek())) advance();
  std::string text = source.substr(start, current - start);
  TokenKind kind =
      keywords.count(text) ? keywords.at(text) : LiteralKind::Identifier;
  return Token(kind, source.substr(start, current - start), position, line);
}

std::optional<Token> Lexer::otherToken() {
  char c = peek();
  TokenKind kind = OtherKind::Bad;
  std::any literal = nullptr;
  std::string lexme = std::to_string(c);

  switch (c) {
    case '(':
      kind = OtherKind::L_Paren;
      break;
    case ')':
      kind = OtherKind::R_Paren;
      break;
    case '[':
      kind = OtherKind::L_Bracket;
      break;
    case ']':
      kind = OtherKind::R_Bracket;
      break;
    case '{':
      kind = OtherKind::L_Brace;
      break;
    case '}':
      kind = OtherKind::R_Brace;
      break;
    case ':':
      kind = OperatorKind::Colon;
      break;
    case '?':
      kind = OperatorKind::Question_Mark;
      break;
  }

  if (std::holds_alternative<OtherKind>(kind) &&
      std::get<OtherKind>(kind) != OtherKind::Bad) {
    advance();
    return Token(kind, lexme, position, line, literal);
  }

  switch (c) {
    case '+':
      kind = match('+')
                 ? OperatorKind::Increment
                 : (match('=') ? OperatorKind::PlusEq : OperatorKind::Plus);
      break;
    case '-':
      kind = match('-') ? OperatorKind::Decrement
                        : (match('>') ? OperatorKind::Arrow
                                      : (match('=') ? OperatorKind::MinusEq
                                                    : OperatorKind::Minus));
      break;
    case '*':
      kind = match('*')
                 ? OperatorKind::Power
                 : (match('=') ? OperatorKind::StarEq : OperatorKind::Star);
      break;
    case '/':
      if (match('/'))
        while (peek() != '\n') advance();
      kind = match('=') ? OperatorKind::SlashEq : OperatorKind::Slash;
      break;
    case '%':
      kind = match('=') ? OperatorKind::ModEq : OperatorKind::Mod;
      break;
    case '|':
      if (match('|')) kind = OperatorKind::Or;
      break;
    case '&':
      if (match('&')) kind = OperatorKind::And;
      break;
    case '>':
      kind = match('=') ? OperatorKind::GreaterEq : OperatorKind::Greater;
      break;
    case '<':
      kind = match('=') ? OperatorKind::LessEq : OperatorKind::Less;
      break;
    case '=':
      kind = match('=') ? OperatorKind::EqEq : OperatorKind::Equals;
      break;
    case '!':
      kind = match('=') ? OperatorKind::NotEq : OperatorKind::Bang;
      break;
    case '"':
      advance();
      return string();
    default:
      return std::nullopt;
  }
  advance();
  advance();
  return Token(kind, source.substr(start, current - start), position, line,
               literal);
}

char Lexer::peek(size_t offset) {
  return current + offset >= source.length() ? '\0'
                                             : source.at(current + offset);
}
char Lexer::advance() {
  position++;
  current++;
  return source.at(current - 1);
}

bool Lexer::match(char expected, size_t offset) {
  return !(isAtEnd() || (source.at(current) != expected));
}

bool Lexer::isAtEnd() { return current >= source.length(); }
}  // namespace ktpp::lexer