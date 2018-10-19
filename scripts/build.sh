#!/bin/bash

mode="$1"

generator="Unix Makefiles"
projRoot="$(dirname "$0")/.."
buildDir="$projRoot/build"
SEP="----------------------------------"

[ -d "$buildDir" ] || mkdir "$buildDir"

pushd "$buildDir"

# run conan
conan install .. -s build_type=$mode
echo "conan returned $?"
echo $SEP

# cmake gen
cmake .. -G "$generator" -DCMAKE_BUILD_TYPE=$mode
echo "cmake returned $?"
echo $SEP

#cmake build
cmake --build . --config $mode
echo "cmake returned $?"
echo $SEP

popd
