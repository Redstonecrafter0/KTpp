#include "Logger/Logger.hh"
#include <iostream>
#include <sstream>

int main() {
  std::stringstream ss1;
  std::stringstream ss2;
  auto logger1 =
      ktpp::logger::create_logger(&ss1, ktpp::logger::LogLevel::Info, false);
  auto logger2 =
      ktpp::logger::create_logger(&ss2, ktpp::logger::LogLevel::Debug);
  auto logger3 = ktpp::logger::sourced_logger(std::move(logger2), "logger2");
  auto combined =
      ktpp::logger::combine_loggers(std::vector<ktpp::logger::Logger *>{
          logger1.release(), logger3.release()});

  combined->Log(ktpp::logger::LogLevel::Debug, "Debug message");
  combined->Log(ktpp::logger::LogLevel::Info, "Info message");
  combined->Log(ktpp::logger::LogLevel::Warn, "Warn message");
  combined->Log(ktpp::logger::LogLevel::Error, "Error message");
  combined->Log(ktpp::logger::LogLevel::Severe, "Severe message");
  combined->Log(ktpp::logger::LogLevel::Info, "Test", "Info message");
  combined->Log(ktpp::logger::LogLevel::Warn, "Test", "Warn message");
  combined->Log(ktpp::logger::LogLevel::Error, "Test", "Error message");
  combined->Log(ktpp::logger::LogLevel::Severe, "Test", "Severe message");

  std::cout << ss1.str() << std::endl;
  std::cout << ss2.str() << std::endl;
}