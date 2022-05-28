#include "helpers.hh"

void test_identifier(std::string str, std::string real) {
  auto value = test_token(str, LiteralKind::Identifier);
  std::cout << value.type().name() << std::endl;
  ASSERT(std::any_cast<std::string>(value) == real)
}

int main() {
  try {
    test_identifier("foo", "foo");
    test_identifier("foo_bar", "foo_bar");
    std::string ids[] = {"foo", "bar", "bazz"};
    auto tokens = eachToken("foo bar bazz");
    for (int i = 0; i < 3; i++) {
      ASSERT(std::any_cast<std::string>(tokens[i].literal) == ids[i])
    };
  } catch (std::exception a) {
    std::cout << a.what() << std::endl;
    return 1;
  }
  return 0;
}