#include "Lexer.hh"

#include <functional>
#include <iostream>

namespace ktpp::lexer {
Lexer::Lexer(logger::Logger *logger, std::string filePath, std::string source)
    : logger(logger), filePath(filePath), source(source) {}
void Lexer::lex() {
  while (!isAtEnd()) {
    start = current;
    try {
      tokenize();
    } catch (std::exception e) {
      emit(logger::LogLevel::Error, e.what());
      hadError = true;
      break;
    }
  }

  tokens.push_back(Token(OtherKind::Eof, "", line, current, nullptr));
}

void error(std::string e) { throw std::runtime_error(e); }

bool is_identifier_start(char c) { return isalpha(c) || c == '_'; }

bool is_identifier_part(char c) { return is_identifier_start(c) || isdigit(c); }

bool is_number_part(char c) {
  return isdigit(c) || c == '.' || c == 'e' || c == '_' || c == '-';
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

  if (is_identifier_start(c)) {
    tokens.push_back(identifier());
    return;
  } else if (isdigit(c)) {
    tokens.push_back(number());
    return;
  }
  error("Unexpected character '" + std::to_string(c) + "'.");
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
    if (!is_number_part(current))
      break;
    lexme += current;
    advance();
    if (current >= '0' && current <= '9')
      n = current - '0';
    else if (current == '_') {
      continue;
    } else if (current == '.') {
      if (is_float)
        error("Invalid float literal");
      is_float = true;
      float_val = (double_t)int_val;
      continue;
    } else if (current == 'e') {
      is_power = true;
      break;
    } else
      error("Invalid number literal");
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
          error("Invalid number literal with multiple negatives");
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
    if (power == 0)
      emit(logger::LogLevel::Warn, "Number literal with power of 0");

    float_val = float_val * std::pow(10, (double_t)power);
  }

  if (is_float)
    return Token(LiteralKind::Float, lexme, position, line, float_val);
  else
    return Token(LiteralKind::Int, lexme, position, line, int_val);
}

Token Lexer::string() {
  std::string lexme;
  int start_line = line;
  while ((peek() != '"') && !isAtEnd()) {
    if (peek() == '\n')
      line++;
    lexme += peek();
    advance();
  }

  if (isAtEnd()) {
    emit(logger::LogLevel::Error, "Unterminated string in " + filePath + ":" +
                                      std::to_string(start_line));
    error("Unterminated string");
  }
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

  if (others.contains(lexme + c)) {
    advance();
    return Token(others.at(lexme + c), lexme + c, position, line, literal);
  }

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

void Lexer::emit(logger::LogLevel level, std::string message) {
  logger->Log(level, "Lexer",
              message + " " + filePath + ":" + std::to_string(line));
}
} // namespace ktpp::lexer