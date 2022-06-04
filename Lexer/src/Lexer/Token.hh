#pragma once
#include <any>
#include <string>
#include <variant>

#include "Diagnostics/Diagnostics.hh"

namespace ktpp::lexer {
enum class OperatorKind {
  // Arithmetic
  Power,
  Increment,
  Decrement,

  Star,
  Slash,
  Mod,

  Minus,
  Plus,

  // Bitshift
  Rsh,
  Lsh,

  // Relational
  Greater,
  GreaterEq,
  Less,
  LessEq,
  EqEq,
  NotEq,

  // Bitwise
  And,
  Xor,
  Or,

  // || &&
  LogicOr,
  LogicAnd,

  // Assignment
  Equals,
  PlusEq,
  MinusEq,
  StarEq,
  SlashEq,
  PowerEq,
  ModEq,
  XorEq,
  OrEq,
  AndEq,
  LshEq,
  RshEq,

  // Strict Unary
  Inv,
  Bang,
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
  Return,
  Break,
  Continue,
  Interface,
  This,
};
enum class LiteralKind { Int, Float, Bool, String, Identifier };
enum class OtherKind {
  Dot,
  Colon,
  Semicolon,
  Question_Mark,
  Arrow,
  Comma,
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
  diagnostics::TextSpan span;
  std::any literal;

  Token(TokenKind kind, std::string lexeme, diagnostics::TextSpan span,
        std::any literal = nullptr) noexcept
      : kind{kind}, lexeme(lexeme), span(span), literal(literal) {}
};
}  // namespace ktpp::lexer