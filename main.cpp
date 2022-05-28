#include <iostream>

#include "Lexer/Lexer.hh"

int main() {
  while (true) {
    std::string source;
    std::cin >> source;
    auto lexer = ktpp::lexer::Lexer(source);
    lexer.lex();

    if (lexer.hadError) continue;
    for (auto t : lexer.tokens)
      std::cout << "Lexeme: " << t.lexeme << "\nPosition: " << t.position
                << "\n"
                << std::endl;
  }
}