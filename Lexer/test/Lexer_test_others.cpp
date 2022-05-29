#include "helpers.cc"

void test_other(std::string str, OtherKind real) {
  auto value = test_token(str, real);
}

void test() {
  test_other("(", OtherKind::L_Paren);
  test_other(")", OtherKind::R_Paren);
  test_other("[", OtherKind::L_Bracket);
  test_other("]", OtherKind::R_Bracket);
  test_other("{", OtherKind::L_Brace);
  test_other("}", OtherKind::R_Brace);
  test_other(",", OtherKind::Comma);
  test_other(";", OtherKind::Semicolon);
  test_other(":", OtherKind::Colon);
  test_other(".", OtherKind::Dot);
  test_other("->", OtherKind::Arrow);
  test_other("?", OtherKind::Question_Mark);
}

TEST(test();)