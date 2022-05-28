#include "helpers.hh"

void test_decl() {
  auto tokens = eachToken("var mut a = 1 + 2");
  auto expected = std::vector<Token>({
      Token(KeywordKind::Var, "var", 0, 0),
      Token(KeywordKind::Mut, "mut", 0, 0),
      Token(LiteralKind::Identifier, "a", 0, 0),
      Token(OperatorKind::Equals, "=", 0, 0),
      Token(LiteralKind::Int, "1", 0, 0),
      Token(OperatorKind::Plus, "+", 0, 0),
      Token(LiteralKind::Int, "2", 0, 0),
      Token(OtherKind::Eof, "", 0, 0),
  });

  for (auto i = 0; i < expected.size(); i++) {
    ASSERT(tokens[i].kind == expected[i].kind);
    ASSERT_EQ(tokens[i].lexeme, expected[i].lexeme);
  }
}

void test_fndef() {
  auto tokens = eachToken("fn foo<K,V>(t: Type) -> int { return \"foo\" }");
  auto expected = std::vector<Token>({
      Token(KeywordKind::Fn, "fn", 0, 0),
      Token(LiteralKind::Identifier, "foo", 0, 0),
      Token(OperatorKind::Less, "<", 0, 0),
      Token(LiteralKind::Identifier, "K", 0, 0),
      Token(OtherKind::Comma, ",", 0, 0),
      Token(LiteralKind::Identifier, "V", 0, 0),
      Token(OperatorKind::Greater, ">", 0, 0),
      Token(OtherKind::L_Paren, "(", 0, 0),
      Token(LiteralKind::Identifier, "t", 0, 0),
      Token(OperatorKind::Colon, ":", 0, 0),
      Token(LiteralKind::Identifier, "Type", 0, 0),
      Token(OtherKind::R_Paren, ")", 0, 0),
      Token(OperatorKind::Arrow, "->", 0, 0),
      Token(LiteralKind::Identifier, "int", 0, 0),
      Token(OtherKind::L_Brace, "{", 0, 0),
      Token(KeywordKind::Return, "return", 0, 0),
      Token(LiteralKind::String, "\"foo\"", 0, 0),
      Token(OtherKind::R_Brace, "}", 0, 0),
      Token(OtherKind::Eof, "", 0, 0),
  });

  for (auto i = 0; i < expected.size(); i++) {
    ASSERT(tokens[i].kind == expected[i].kind);
    ASSERT_EQ(tokens[i].lexeme, expected[i].lexeme);
  }
}

void test_oop() {
  auto tokens = eachToken("class Foo {  } interface Bar {  }");
  auto expected = std::vector<Token>({
      Token(KeywordKind::Class, "class", 0, 0),
      Token(LiteralKind::Identifier, "Foo", 0, 0),
      Token(OtherKind::L_Brace, "{", 0, 0),
      Token(OtherKind::R_Brace, "}", 0, 0),
      Token(KeywordKind::Interface, "interface", 0, 0),
      Token(LiteralKind::Identifier, "Bar", 0, 0),
      Token(OtherKind::L_Brace, "{", 0, 0),
      Token(OtherKind::R_Brace, "}", 0, 0),
      Token(OtherKind::Eof, "", 0, 0),
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