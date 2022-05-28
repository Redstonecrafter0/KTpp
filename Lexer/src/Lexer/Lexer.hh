#pragma once
#include "Logger/Logger.hh"
#include <map>
#include <optional>
#include <vector>

#include "Token.hh"

namespace ktpp::lexer {
class Lexer {
public:
  bool hadError = false;
  std::vector<Token> tokens;
  Lexer(logger::Logger *logger, std::string filePath, std::string source);
  void lex();
  void tokenize();

private:
  logger::Logger *logger;
  std::string filePath;
  std::string source;

  size_t line = 1, position = 0, current = 0, start = 0;
  std::map<std::string, TokenKind> keywords{
      {"var", KeywordKind::Var},
      {"mut", KeywordKind::Mut},
      {"for", KeywordKind::For},
      {"while", KeywordKind::While},
      {"do", KeywordKind::Do},
      {"if", KeywordKind::If},
      {"else", KeywordKind::Else},
      {"fn", KeywordKind::Fn},
      {"class", KeywordKind::Class},
      {"true", KeywordKind::True},
      {"false", KeywordKind::False},
      {"type", KeywordKind::Type},
      {"switch", KeywordKind::Switch},
      {"async", KeywordKind::Async},
      {"await", KeywordKind::Await},
      {"return", KeywordKind::Return},
      {"interface", KeywordKind::Interface},
      {"break", KeywordKind::Break},
      {"continue", KeywordKind::Continue},
  };

  std::map<std::string, OperatorKind> operators{
      {"+", OperatorKind::Plus},       {"-", OperatorKind::Minus},
      {"*", OperatorKind::Star},       {"/", OperatorKind::Slash},
      {"%", OperatorKind::Mod},        {"=", OperatorKind::Equals},
      {"==", OperatorKind::EqEq},      {"!=", OperatorKind::NotEq},
      {"<", OperatorKind::Less},       {"<=", OperatorKind::LessEq},
      {">", OperatorKind::Greater},    {">=", OperatorKind::GreaterEq},
      {"&&", OperatorKind::LogicAnd},  {"||", OperatorKind::LogicOr},
      {"++", OperatorKind::Increment}, {"--", OperatorKind::Decrement},
      {"+=", OperatorKind::PlusEq},    {"-=", OperatorKind::MinusEq},
      {"*=", OperatorKind::StarEq},    {"/=", OperatorKind::SlashEq},
      {"%=", OperatorKind::ModEq},     {"&=", OperatorKind::AndEq},
      {"|=", OperatorKind::OrEq},      {"**", OperatorKind::Power},
      {"**=", OperatorKind::PowerEq},  {"|", OperatorKind::Or},
      {"&", OperatorKind::And},        {"^=", OperatorKind::XorEq},
      {"^", OperatorKind::Xor},        {"<<", OperatorKind::Lsh},
      {">>", OperatorKind::Rsh},       {"<<=", OperatorKind::LshEq},
      {">>=", OperatorKind::RshEq},    {"!", OperatorKind::Bang},
      {"~", OperatorKind::Inv}};

  std::map<std::string, OtherKind> others{
      {"->", OtherKind::Arrow},    {"?", OtherKind::Question_Mark},
      {":", OtherKind::Colon},     {"(", OtherKind::L_Paren},
      {")", OtherKind::R_Paren},   {"{", OtherKind::L_Brace},
      {"}", OtherKind::R_Brace},   {"[", OtherKind::L_Bracket},
      {"]", OtherKind::R_Bracket}, {",", OtherKind::Comma},
      {".", OtherKind::Dot},       {";", OtherKind::Semicolon},
  };

  Token number();
  Token string();
  Token identifier();
  std::optional<Token> otherToken();

  char peek(size_t offset = 0);
  char advance();
  bool advance(bool);
  bool match(char expected, size_t offset = 0);
  bool isAtEnd();
  void emit(logger::LogLevel, std::string);
};
} // namespace ktpp::lexer