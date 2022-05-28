#include "helpers.hh"

void test_operator(std::string str, OperatorKind real) {
  auto value = test_token(str, real);
}

void test() {
  test_operator("+", OperatorKind::Plus);
  test_operator("-", OperatorKind::Minus);
  test_operator("*", OperatorKind::Star);
  test_operator("/", OperatorKind::Slash);
  test_operator("%", OperatorKind::Mod);
  test_operator("=", OperatorKind::Equals);
  test_operator("==", OperatorKind::EqEq);
  test_operator("!=", OperatorKind::NotEq);
  test_operator("<", OperatorKind::Less);
  test_operator("<=", OperatorKind::LessEq);
  test_operator(">", OperatorKind::Greater);
  test_operator(">=", OperatorKind::GreaterEq);
  test_operator("&&", OperatorKind::LogicAnd);
  test_operator("||", OperatorKind::LogicOr);
  test_operator("!", OperatorKind::Bang);
  test_operator("++", OperatorKind::Increment);
  test_operator("--", OperatorKind::Decrement);
  test_operator("+=", OperatorKind::PlusEq);
  test_operator("-=", OperatorKind::MinusEq);
  test_operator("*=", OperatorKind::StarEq);
  test_operator("/=", OperatorKind::SlashEq);
  test_operator("%=", OperatorKind::ModEq);
  test_operator("&=", OperatorKind::AndEq);
  test_operator("|=", OperatorKind::OrEq);
  test_operator("**", OperatorKind::Power);
  test_operator("**=", OperatorKind::PowerEq);
  test_operator("|", OperatorKind::Or);
  test_operator("&", OperatorKind::And);
  test_operator("^", OperatorKind::Xor);
  test_operator("^=", OperatorKind::XorEq);
  test_operator("<<", OperatorKind::Lsh);
  test_operator("<<=", OperatorKind::LshEq);
  test_operator(">>", OperatorKind::Rsh);
  test_operator(">>=", OperatorKind::RshEq);
  test_operator("~", OperatorKind::Inv);
}

TEST(test();)