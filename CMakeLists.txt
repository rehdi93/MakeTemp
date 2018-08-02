project(XmakeTemp)
cmake_minimum_required(VERSION 3.0.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(xmktemp mktemp.cpp)
target_link_libraries(xmktemp ${CONAN_LIBS})
target_compile_features(xmktemp PRIVATE cxx_std_17)
target_include_directories(xmktemp PRIVATE F:/DEV/RedHands/Common)