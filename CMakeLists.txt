cmake_minimum_required(VERSION 3.0.0)
project(cpp-modern-development)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
  # warning level 4 and all warnings as errors
  add_compile_options(/W4 /WX)
else()
  # lots of warnings and all warnings as errors
  # -pedantic
  add_compile_options(-Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-missing-field-initializers -Wno-unknown-pragmas -O3)
endif()

find_package(Threads)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

file(GLOB cmakelists RELATIVE ${PROJECT_SOURCE_DIR} CONFIGURE_DEPENDS */*/CMakeLists.txt)
foreach(cmakelist ${cmakelists})
  get_filename_component(dir ${cmakelist} DIRECTORY)
  message(STATUS ${dir})
  add_subdirectory(${PROJECT_SOURCE_DIR}/${dir})
endforeach()