#include "helpers.cc"

void test_identifier(std::string str, std::string real) {
  auto value = test_token(str, LiteralKind::Identifier);
  ASSERT(std::any_cast<std::string>(value) == real)
}

void test() {
  test_identifier("foo", "foo");
  test_identifier("foo_bar", "foo_bar");
  std::string ids[] = {"foo", "bar", "bazz"};
  auto tokens = eachToken("foo bar bazz");
  for (int i = 0; i < 3; i++) {
    ASSERT(std::any_cast<std::string>(tokens[i].literal) == ids[i])
  };
};

TEST(test();)