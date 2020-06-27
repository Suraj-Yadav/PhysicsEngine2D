#!/bin/bash

set -e
CMAKE_GENERATOR="MinGW Makefiles"

case $OSTYPE in
    darwin*)
        CMAKE_GENERATOR="Unix Makefiles"
    ;;
    *)
    ;;
esac


rm -rf build;
cmake -S . -B build/Debug -DCMAKE_BUILD_TYPE=Debug -G "$CMAKE_GENERATOR"
cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release -G "$CMAKE_GENERATOR"
cmake -S . -B build/Profile -DCMAKE_BUILD_TYPE=Profile -G "$CMAKE_GENERATOR"
