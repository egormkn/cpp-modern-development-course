cmake_minimum_required(VERSION 3.0.0)

add_executable(transport_manager_f
  main.cpp
  svg.cpp)

target_compile_definitions(transport_manager_f PRIVATE
  _GLIBCXX_DEBUG 
  _GLIBCXX_DEBUG_PEDANTIC 
  _LIBCPP_DEBUG=1)

target_compile_options(transport_manager_f PRIVATE
  -fsanitize=undefined
  -g                          # Include file names and line numbers
  -fno-omit-frame-pointer
  -fno-optimize-sibling-calls)

target_link_libraries(transport_manager_f
  -fsanitize=undefined
)

# Run with 
# ASAN_SYMBOLIZER_PATH=/usr/lib/llvm-6.0/bin/llvm-symbolizer
# UBSAN_SYMBOLIZER_PATH=/usr/lib/llvm-6.0/bin/llvm-symbolizer