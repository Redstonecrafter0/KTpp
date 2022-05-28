#include "helpers.hh"

void test_string(std::string str, std::string real) {
  auto value = test_token(str, LiteralKind::String);
  ASSERT(std::any_cast<std::string>(value) == real)
}

void test() {
  test_string("\"\"", "");
  test_string("\"foo\"", "foo");
  test_string("\"foo bar\"", "foo bar");
}

TEST(test();)