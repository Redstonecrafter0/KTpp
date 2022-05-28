#include "helpers.hh"

void test_int(std::string str, int64_t real) {
  auto value = test_token(str, LiteralKind::Int);
  ASSERT_EQ(std::any_cast<int64_t>(value), real)
}

void test_float(std::string str, double_t real) {
  auto value = test_token(str, LiteralKind::Float);
  ASSERT_EQ(std::any_cast<double_t>(value), real)
}

void test() {
  test_int("123", 123);
  test_int("1_234", 1234);
  test_int("1e10", 1e10);
  test_float("123.456", 123.456);
  test_float("123e-1", 123e-1);
  test_float("1_234e-1_0", 1234e-10);
}

TEST(test();)