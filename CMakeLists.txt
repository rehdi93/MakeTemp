project(MakeTemp)
cmake_minimum_required(VERSION 3.0.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(makeTemp mktemp.cpp)
target_link_libraries(makeTemp ${CONAN_LIBS})
target_compile_features(makeTemp PRIVATE cxx_std_17)
