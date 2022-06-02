#include <iostream>
#include <sstream>

#include "Lexer/Lexer.hh"
#include "Parser/Parser.hh"

int main() {
  while (true) {
    std::string source;
    getline(std::cin, source);
    auto logger =
        ktpp::logger::create_logger(&std::cout, ktpp::logger::LogLevel::Info);

    auto parser = ktpp::parser::Parser(logger.get(), "console", source);
    std::vector<std::unique_ptr<ktpp::parser::Stmt>> stmts = parser.parse();

    if (parser.hadError) continue;
    for (auto& stmt : stmts)
      std::cout << "Statement: " << typeid(stmt).name() << std::endl;
  }
}