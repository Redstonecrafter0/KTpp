add_library(
  Parser
  Parser/Parser.cpp
  Parser/Parser.hh
)

target_include_directories(Parser PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(Parser Lexer)