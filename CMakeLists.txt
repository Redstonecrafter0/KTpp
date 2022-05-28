cmake_minimum_required(VERSION 3.0.0)
project(ktpp VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS  "-g")

include(CTest)
enable_testing()
add_subdirectory(Lexer)
add_subdirectory(Logger)

add_executable(ktpp main.cpp)

target_link_libraries(${PROJECT_NAME} Lexer)