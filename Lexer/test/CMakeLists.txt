file(GLOB_RECURSE TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_library(LexerTesters "helpers.cc")
target_link_libraries(LexerTesters Lexer)


foreach(FILE ${TESTS})
  string(REGEX REPLACE "[./]" "_" NAME ${FILE})
    add_executable(${NAME} ${FILE})
    target_link_libraries(${NAME} Lexer)
    target_link_libraries(${NAME} LexerTesters)
    add_test(NAME ${NAME} COMMAND ${NAME})
endforeach(FILE ${TESTS})

enable_testing()