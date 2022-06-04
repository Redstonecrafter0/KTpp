#include <iostream>

#include "Parser/Parser.hh"

std::vector<Token> lex(ktpp::diagnostics::Diagnostics* diagnostics,
                       std::string source) {
  auto lexer = ktpp::lexer::Lexer(diagnostics, "mockfile", source);
  lexer.lex();
  return lexer.tokens;
}

int main() {
  std::string s1 = "for var i = 0; i < 10; i++ { }";
  std::string s2 = "for var i in [1, 2, 3] { }";
  auto logger =
      ktpp::logger::create_logger(&std::cout, ktpp::logger::LogLevel::Info);
  auto diagnostics = ktpp::diagnostics::Diagnostics(logger.get());
  auto parser =
      ktpp::parser::Parser(logger.get(), "mockfile", lex(&diagnostics, s1));
  parser.parse();
  parser =
      ktpp::parser::Parser(logger.get(), "mockfile", lex(&diagnostics, s2));
  parser.parse();
  return 0;
}