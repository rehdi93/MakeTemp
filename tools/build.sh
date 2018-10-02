#!/bin/bash

mode="$1"

projRoot="$(dirname "$0")/.."
buildDir="$projRoot/build"
SEP="----------------------------------"

pushd "$buildDir"

# run conan
conan install .. -s build_type=$mode
echo "conan returned $?"
echo $SEP

# cmake gen
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$mode
echo "cmake returned $?"
echo $SEP

#cmake build
cmake --build . --config $mode
echo "cmake returned $?"
echo $SEP

popd
