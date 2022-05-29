#include "helpers.hh"

void test_decl() {
  auto tokens = eachToken("var mut a = 1 + 2");
  auto expected = std::vector<Token>({
      token(KeywordKind::Var, "var"),
      token(KeywordKind::Mut, "mut"),
      token(LiteralKind::Identifier, "a"),
      token(OperatorKind::Equals, "="),
      token(LiteralKind::Int, "1"),
      token(OperatorKind::Plus, "+"),
      token(LiteralKind::Int, "2"),
      token(OtherKind::Eof, ""),
  });

  for (auto i = 0; i < expected.size(); i++) {
    ASSERT(tokens[i].kind == expected[i].kind);
    ASSERT_EQ(tokens[i].lexeme, expected[i].lexeme);
  }
}

void test_fndef() {
  auto tokens = eachToken("fn foo<K,V>(t: Type) -> int { return \"foo\" }");
  auto expected = std::vector<Token>({
      token(KeywordKind::Fn, "fn"),
      token(LiteralKind::Identifier, "foo"),
      token(OperatorKind::Less, "<"),
      token(LiteralKind::Identifier, "K"),
      token(OtherKind::Comma, ","),
      token(LiteralKind::Identifier, "V"),
      token(OperatorKind::Greater, ">"),
      token(OtherKind::L_Paren, "("),
      token(LiteralKind::Identifier, "t"),
      token(OtherKind::Colon, ":"),
      token(LiteralKind::Identifier, "Type"),
      token(OtherKind::R_Paren, ")"),
      token(OtherKind::Arrow, "->"),
      token(LiteralKind::Identifier, "int"),
      token(OtherKind::L_Brace, "{"),
      token(KeywordKind::Return, "return"),
      token(LiteralKind::String, "\"foo\""),
      token(OtherKind::R_Brace, "}"),
      token(OtherKind::Eof, ""),
  });

  for (auto i = 0; i < expected.size(); i++) {
    ASSERT(tokens[i].kind == expected[i].kind);
    ASSERT_EQ(tokens[i].lexeme, expected[i].lexeme);
  }
}

void test_oop() {
  auto tokens = eachToken("class Foo {  } interface Bar {  }");
  auto expected = std::vector<Token>({
      token(KeywordKind::Class, "class"),
      token(LiteralKind::Identifier, "Foo"),
      token(OtherKind::L_Brace, "{"),
      token(OtherKind::R_Brace, "}"),
      token(KeywordKind::Interface, "interface"),
      token(LiteralKind::Identifier, "Bar"),
      token(OtherKind::L_Brace, "{"),
      token(OtherKind::R_Brace, "}"),
      token(OtherKind::Eof, ""),
  });

  for (auto i = 0; i < expected.size(); i++) {
    ASSERT(tokens[i].kind == expected[i].kind);
    ASSERT_EQ(tokens[i].lexeme, expected[i].lexeme);
  }
}

void test() {
  test_decl();
  test_fndef();
}

TEST(test();)