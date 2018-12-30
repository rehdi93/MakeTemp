cmake_minimum_required(VERSION 3.8.0)
project(MakeTemp
  VERSION 1.1.5
)

set(MAKETEMP_EXE maketemp)
configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_SOURCE_DIR}/config.h)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(maketemp makeTemp.cpp program.cpp)
target_link_libraries(maketemp ${CONAN_LIBS})
target_include_directories(maketemp PRIVATE 3rdparty)

set_target_properties(
  maketemp PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)

# GCC (and maybe clang? untested) requires linking an external lib 
# to use std::filesystem
if(NOT MSVC)
  target_link_libraries(maketemp stdc++fs)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
