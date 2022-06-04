#include <iostream>

#include "Diagnostics/Diagnostics.hh"

using namespace ktpp;

int main() {
  auto logger1 =
      logger::create_logger(&std::cout, logger::LogLevel::Info, true);
  auto diagnostics = diagnostics::Diagnostics(logger1.get());
  diagnostics.log(logger::LogLevel::Info, "Hello, world!");
  // add source file
  diagnostics.addFile("/path/to/file.ktpp", "var a = \"test");
  // emit error
  diagnostics.emit(logger::LogLevel::Error, "Error message",
                   diagnostics::TextSpan("/path/to/file.ktpp", 1, 0, 8, 13));
  return 0;
}