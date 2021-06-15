cmake_minimum_required(VERSION 3.0.0)

add_executable(domains_sanitized 
  main.cpp)

target_compile_definitions(domains_sanitized PRIVATE
  _GLIBCXX_DEBUG 
  _GLIBCXX_DEBUG_PEDANTIC 
  _LIBCPP_DEBUG=1)

target_compile_options(domains_sanitized PRIVATE
  -fsanitize=address
  -g                          # Include file names and line numbers
  -fno-omit-frame-pointer
  -fno-optimize-sibling-calls)

target_link_libraries(domains_sanitized
  -fsanitize=address
)

# Run with ASAN_SYMBOLIZER_PATH=/usr/lib/llvm-6.0/bin/llvm-symbolizer