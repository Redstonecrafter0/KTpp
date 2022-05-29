#include "helpers.cc"

void test_keyword(std::string str, KeywordKind real) {
  auto value = test_token(str, real);
}

void test() {
  test_keyword("var", KeywordKind::Var);
  test_keyword("mut", KeywordKind::Mut);
  test_keyword("for", KeywordKind::For);
  test_keyword("while", KeywordKind::While);
  test_keyword("do", KeywordKind::Do);
  test_keyword("if", KeywordKind::If);
  test_keyword("else", KeywordKind::Else);
  test_keyword("fn", KeywordKind::Fn);
  test_keyword("class", KeywordKind::Class);
  test_keyword("true", KeywordKind::True);
  test_keyword("false", KeywordKind::False);
  test_keyword("type", KeywordKind::Type);
  test_keyword("switch", KeywordKind::Switch);
  test_keyword("async", KeywordKind::Async);
  test_keyword("await", KeywordKind::Await);
  test_keyword("return", KeywordKind::Return);
  test_keyword("interface", KeywordKind::Interface);
};

TEST(test();)