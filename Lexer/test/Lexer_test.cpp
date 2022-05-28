#include "Lexer/Lexer.hh"
#include <excpt.h>
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
using namespace ktpp::lexer;

std::any test_token(std::string str) {
  auto lexer = Lexer(str);
  lexer.tokenize();
  ASSERT(!lexer.hadError)
  auto tokens = lexer.tokens;
  ASSERT(tokens.size() == 1)
  auto kind = tokens[0].kind;
  ASSERT(std::holds_alternative<LiteralKind>(kind))
  ASSERT(std::get<LiteralKind>(kind) == LiteralKind::Int)
  std::cout << tokens[0].literal.type().name() << std::endl;
  return tokens[0].literal;
}

void test_int(std::string str, int64_t real) {
  auto value = test_token(str);
  ASSERT_EQ(std::any_cast<int64_t>(value), real)
}

void test_float(std::string str, double_t real) {
  auto value = test_token(str);
  ASSERT_EQ(std::any_cast<double_t>(value), real)
}

int main() {
  try {
    test_int("123", 123);
    test_int("1_234", 1234);
    test_int("1e10", 1e10);
    test_float("123.456", 123.456);
    test_float("123e-1", 123e-1);
    test_float("1_234e-1_0", 1234e-10);
  } catch (std::exception a) {
    std::cout << a.what() << std::endl; // ok, off by 1
    return 1;
  }
  return 0;
}