#include <iostream>
#include <sstream>

#include "Lexer/Lexer.hh"
#include "Parser/Parser.hh"

int main() {
  while (true) {
    std::string source = "var a = 1";
    getline(std::cin, source);
    auto logger =
        ktpp::logger::create_logger(&std::cout, ktpp::logger::LogLevel::Info);
    auto diagnostics = ktpp::diagnostics::Diagnostics(logger.get());
    auto lexer = ktpp::lexer::Lexer(&diagnostics, "mockfile", source);
    lexer.lex();
    auto parser = ktpp::parser::Parser(logger.get(), "console", lexer.tokens);
    std::vector<std::unique_ptr<ktpp::parser::Stmt>> stmts = parser.parse();

    if (parser.hadError) continue;
    for (auto& stmt : stmts) {
      std::string name = std::string(typeid(stmt.get()).name());
      std::cout << "Statement: " << name << std::endl;
    }
  }
}