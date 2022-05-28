#pragma once
#include <map>
#include <optional>
#include <vector>

#include "Token.hh"

namespace ktpp::lexer {
class Lexer {
 public:
  bool hadError = false;
  std::vector<Token> tokens;
  Lexer(std::string source);
  void lex();
  void tokenize();

 private:
  std::string source;
  size_t line = 1, position = 0, current = 0, start = 0;
  std::map<std::string, TokenKind> keywords{
      {"var", KeywordKind::Var},       {"mut", KeywordKind::Mut},
      {"for", KeywordKind::For},       {"while", KeywordKind::While},
      {"do", KeywordKind::Do},         {"if", KeywordKind::If},
      {"else", KeywordKind::Else},     {"fn", KeywordKind::Fn},
      {"class", KeywordKind::Class},   {"true", KeywordKind::True},
      {"false", KeywordKind::False},   {"type", KeywordKind::Type},
      {"switch", KeywordKind::Switch}, {"async", KeywordKind::Async},
      {"await", KeywordKind::Await}};

  Token number();
  Token string();
  Token identifier();
  std::optional<Token> otherToken();

  char peek(size_t offset = 0);
  char advance();
  bool match(char expected, size_t offset = 0);
  bool isAtEnd();
};
}  // namespace ktpp::lexer