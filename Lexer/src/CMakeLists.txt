add_library(
  Lexer
  Lexer/lexer.cpp
  Lexer/lexer.hh
)

target_include_directories(Lexer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")