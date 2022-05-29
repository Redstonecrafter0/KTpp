#include "Lexer/Lexer.hh"
#include <iostream>
#include <sstream>

int main() {
  while (true) {
    std::string source;
    getline(std::cin, source);
    auto logger =
        ktpp::logger::create_logger(&std::cout, ktpp::logger::LogLevel::Info);

    auto lexer = ktpp::lexer::Lexer(logger.get(), "console", source);
    lexer.lex();

    if (lexer.hadError)
      continue;
    for (auto t : lexer.tokens)
      std::cout << "Lexeme: " << t.lexeme << std::endl;
  }
}