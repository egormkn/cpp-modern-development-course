cmake_minimum_required(VERSION 3.0.0)

add_executable(square main.cpp)
target_compile_features(square PUBLIC cxx_std_17)
