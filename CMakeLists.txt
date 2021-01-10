cmake_minimum_required(VERSION 3.0.0)
project(cpp-modern-development)

file(GLOB cmakelists RELATIVE ${PROJECT_SOURCE_DIR} CONFIGURE_DEPENDS */*/CMakeLists.txt)
foreach(cmakelist ${cmakelists})
  get_filename_component(dir ${cmakelist} DIRECTORY)
  message(STATUS ${dir})
  add_subdirectory(${PROJECT_SOURCE_DIR}/${dir})
endforeach()