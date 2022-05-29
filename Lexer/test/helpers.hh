#include "Lexer/Lexer.hh"
#include <iostream>

#define ASSERT_EQ(a, b)                                                        \
  if (!(a == b)) {                                                             \
    std::cout << "Assertion failed: " << a << " == " << b << std::endl;        \
    exit(1);                                                                   \
  }

#define ASSERT(val)                                                            \
  if (!(val)) {                                                                \
    std::cout << "Assertion " << #val << " failed in " << __FILE__ << ":"      \
              << __LINE__ << std::endl;                                        \
    exit(1);                                                                   \
  }

#define TEST(body)                                                             \
  int main() {                                                                 \
    try {                                                                      \
      body                                                                     \
    } catch (std::exception e) {                                               \
      std::cout << e.what() << std::endl;                                      \
      return 1;                                                                \
    }                                                                          \
  }

using namespace ktpp::lexer;

template <typename TestKind>
std::any test_token(std::string str, TestKind tkind) {
  auto logger =
      ktpp::logger::create_logger(&std::cout, ktpp::logger::LogLevel::Info);
  auto lexer = Lexer(logger.get(), "mockfile", str);
  lexer.lex();
  ASSERT(!lexer.hadError)
  auto tokens = lexer.tokens;
  ASSERT(tokens.size() >= 1)
  auto kind = tokens[0].kind;
  ASSERT(std::holds_alternative<TestKind>(kind))
  ASSERT(std::get<TestKind>(kind) == tkind)
  return tokens[0].literal;
}

auto eachToken(std::string code) {
  auto logger =
      ktpp::logger::create_logger(&std::cout, ktpp::logger::LogLevel::Info);
  auto lexer = Lexer(logger.get(), "mockfile", code);
  lexer.lex();
  ASSERT(!lexer.hadError)
  auto tokens = lexer.tokens;
  int n = 0;
  return lexer.tokens;
}

auto token(TokenKind kind, std::string lexme) {
  return Token(kind, lexme, TextSpan("mockfile", 0, 0, 0));
}