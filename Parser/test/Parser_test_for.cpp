#include "Parser/Parser.hh"
#include <iostream>

int main() {
  std::string s;
  std::cin >> s;
  auto logger =
      ktpp::logger::create_logger(&std::cout, ktpp::logger::LogLevel::Info);
  auto parser = ktpp::parser::Parser(logger.get(), "mockfile", s);

  std::vector<std::unique_ptr<ktpp::parser::Stmt>> stmts = parser.parse();
  for (auto &stmt : stmts)
    std::cout << typeid(*stmt).name() << std::endl;
  return 0;
}