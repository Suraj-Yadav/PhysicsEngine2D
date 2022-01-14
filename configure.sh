#!/bin/bash

set -e
CMAKE_GENERATOR="Unix Makefiles"

case $OSTYPE in
    darwin*)
        CMAKE_GENERATOR="Unix Makefiles"
    ;;
    msys*)
        CMAKE_GENERATOR="MinGW Makefiles"
    ;;
    *)
    ;;
esac

rm -rf build;
for buildType in "Debug" "Release" "Profile"; do
    cmake -S . -B "build/$buildType" "-DCMAKE_BUILD_TYPE=$buildType" -G "$CMAKE_GENERATOR"
done
