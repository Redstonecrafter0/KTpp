file(GLOB_RECURSE TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hh)

foreach(FILE ${HEADERS})
  add_library(LexerTesters ${FILE})
endforeach(FILE ${HEADERS})
target_link_libraries(LexerTesters Lexer)


foreach(FILE ${TESTS})
  string(REGEX REPLACE "[./]" "_" NAME ${FILE})
    add_executable(${NAME} ${FILE})
    target_link_libraries(${NAME} Lexer)
    target_link_libraries(${NAME} LexerTesters)
    add_test(NAME ${NAME} COMMAND ${NAME})
endforeach(FILE ${TESTS})

enable_testing()