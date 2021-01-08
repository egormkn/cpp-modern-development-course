cmake_minimum_required(VERSION 3.0.0)

add_executable(test_rational main.cpp)
target_compile_features(test_rational PUBLIC cxx_std_17)
