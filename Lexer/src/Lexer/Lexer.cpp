#include "Lexer.hh"

#include <functional>
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

bool is_identifier_start(char c) { return isalpha(c) || c == '_'; }

bool is_identifier_part(char c) { return is_identifier_start(c) || isdigit(c); }

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

  if (is_identifier_start(c)) {
    tokens.push_back(identifier());
    return;
  } else if (isdigit(c)) {
    tokens.push_back(number());
    return;
  }
  throw "Unexpected character '" + std::to_string(c) + "'.";
}

Token Lexer::number() {
  int64_t int_val = 0;
  std::string lexme;
  double_t float_val, fraction_size;
  fraction_size = 1;
  bool is_float = false;
  bool is_power = false;
  int i = 0;

  for (;;) {
    int n = 0;
    char current = peek();
    if (current == '\0')
      break;
    lexme += current;
    advance();
    if (current >= '0' && current <= '9')
      n = current - '0';
    else if (current == '_') {
      continue;
    } else if (current == '.') {
      if (is_float)
        throw "Invalid float literal";
      is_float = true;
      float_val = (double_t)int_val;
      continue;
    } else if (current == 'e') {
      is_power = true;
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
  }

  bool neg_power = false;
  if (is_power) {
    int64_t power = 0;
    for (;;) {
      char current = peek();
      if (current == '\0')
        break;
      if (!isdigit(current) && current != '-' && current != '_')
        break;
      lexme += current;
      advance();
      if (current == '_')
        continue;
      if (current == '-') {
        if (neg_power)
          throw "Invalid number literal with multiple negatives";
        neg_power = true;
        continue;
      }
      power *= 10;
      power += current - '0';
    }

    if (neg_power)
      power = -power;
    if (power > 0) {
      if (!is_float)
        int_val = int_val * std::pow(10, power);
    } else if (!is_float) {
      float_val = (double_t)int_val;
      is_float = true;
    }
    float_val = float_val * std::pow(10, (double_t)power);
  }

  if (is_float)
    return Token(LiteralKind::Float, lexme, position, line, float_val);
  else
    return Token(LiteralKind::Int, lexme, position, line, int_val);
}

Token Lexer::string() {
  std::string lexme;
  while ((peek() != '"') && !isAtEnd()) {
    if (peek() == '\n')
      line++;
    lexme += peek();
    advance();
  }

  if (isAtEnd())
    throw "Unterminated string.";
  advance();

  std::string value = lexme;
  return Token(LiteralKind::String, "\"" + value + "\"", position, line, value);
}

Token Lexer::identifier() {
  std::string lexme;
  while (is_identifier_part(peek())) {
    lexme += advance();
  }
  TokenKind kind =
      keywords.contains(lexme) ? keywords.at(lexme) : LiteralKind::Identifier;
  return Token(kind, lexme, position, line, lexme);
}

std::optional<Token> Lexer::otherToken() {
  char c = peek();
  TokenKind kind = OtherKind::Bad;
  std::any literal = nullptr;
  std::string lexme;

  if (c == '\"') {
    advance();
    return string();
  }
  while (operators.contains(lexme + c)) {
    lexme += peek();
    advance();
    c = peek();
    if (c == '\0')
      break;
  }
  if (operators.contains(lexme)) {
    kind = operators.at(lexme);
    return Token(kind, lexme, position, line, literal);
  }
  return std::nullopt;
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

bool Lexer::advance(bool condition) {
  if (condition) {
    advance();
  }
  return condition;
}

bool Lexer::match(char expected, size_t offset) {
  return !(isAtEnd() || current + offset >= source.length() ||
           (source.at(current) != expected));
}

bool Lexer::isAtEnd() { return current >= source.length(); }
} // namespace ktpp::lexer