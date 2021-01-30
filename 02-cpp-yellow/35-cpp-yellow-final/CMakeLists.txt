cmake_minimum_required(VERSION 3.0.0)

set(SOURCES 
  condition_parser.cpp
  condition_parser_test.cpp
  database.cpp
  date.cpp
  main.cpp 
  node.cpp
  token.cpp 
)
add_executable(cpp_yellow_final ${SOURCES})
