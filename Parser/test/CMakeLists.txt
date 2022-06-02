file(GLOB_RECURSE TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(FILE ${TESTS})
  string(REGEX REPLACE "[./]" "_" NAME ${FILE})
    add_executable(${NAME} ${FILE})
    target_link_libraries(${NAME} Parser Lexer)
    add_test(NAME ${NAME} COMMAND ${NAME})
endforeach(FILE ${TESTS})

enable_testing()