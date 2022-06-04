cmake_minimum_required(VERSION 3.0.0)
project(ktpp VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
  set(CMAKE_CXX_FLAGS "/DEBUG /W4")
else()
  set(CMAKE_CXX_FLAGS "-Wall -g -O0")
endif()

include(CTest)
enable_testing()
add_subdirectory(Lexer)
add_subdirectory(Logger)
add_subdirectory(Diagnostics)
add_subdirectory(Parser)

add_executable(ktpp main.cpp)

target_link_libraries(${PROJECT_NAME} Lexer)
target_link_libraries(${PROJECT_NAME} Parser)