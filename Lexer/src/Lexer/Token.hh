#pragma once
#include <any>
#include <string>
#include <variant>

namespace ktpp::lexer {
enum class OperatorKind {
  Plus,
  Minus,
  Star,
  Slash,
  Power,
  Mod,
  LogicOr,
  Or,
  LogicAnd,
  And,
  Equals,
  Increment,
  Decrement,
  PlusEq,
  MinusEq,
  StarEq,
  SlashEq,
  PowerEq,
  ModEq,
  OrEq,
  AndEq,
  EqEq,
  NotEq,
  Greater,
  GreaterEq,
  Less,
  LessEq,
  Bang,
  Colon,
  Question_Mark,
  Arrow
};
enum class KeywordKind {
  While,
  Do,
  For,
  Var,
  Mut,
  True,
  False,
  Type,
  Fn,
  Class,
  If,
  Else,
  Switch,
  Async,
  Await,
};
enum class LiteralKind { Int, Float, Bool, String, Identifier };
enum class OtherKind {
  L_Paren,
  R_Paren,
  L_Bracket,
  R_Bracket,
  L_Brace,
  R_Brace,
  Eof,
  Bad
};
using TokenKind =
    std::variant<OperatorKind, KeywordKind, LiteralKind, OtherKind>;
class Token {
 public:
  TokenKind kind;
  std::string lexeme;
  size_t position, line;
  std::any literal;

  Token(TokenKind kind, std::string lexeme, size_t position,
        size_t line) noexcept
      : kind{kind},
        lexeme(lexeme),
        position(position),
        line(line),
        literal(nullptr) {}

  Token(TokenKind kind, std::string lexeme, size_t position, size_t line,
        std::any literal) noexcept
      : kind{kind},
        lexeme(lexeme),
        position(position),
        line(line),
        literal(literal) {}
};
}  // namespace ktpp::lexer