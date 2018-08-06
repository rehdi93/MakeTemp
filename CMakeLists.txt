project(MakeTemp)
cmake_minimum_required(VERSION 3.8.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(makeTemp makeTemp.cpp program.cpp)
target_link_libraries(makeTemp ${CONAN_LIBS})
# target_compile_features(makeTemp PRIVATE cxx_std_17)
set_target_properties(makeTemp 
PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
)
