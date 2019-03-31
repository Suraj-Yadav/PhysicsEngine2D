#!/bin/bash

set -e
# rm -rf build;
mkdir -p build; cd build
mkdir -p Release Debug Profile

cd Debug; cmake -DCMAKE_BUILD_TYPE=Debug  -G "MinGW Makefiles" ../..; cd ..
cd Release; cmake -DCMAKE_BUILD_TYPE=Release  -G "MinGW Makefiles" ../..; cd ..
cd Profile; cmake -DCMAKE_BUILD_TYPE=Profile  -G "MinGW Makefiles" ../..; cd ..