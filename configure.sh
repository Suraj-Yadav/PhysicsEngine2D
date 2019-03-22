#!/bin/bash

rm -rf build;
mkdir build; cd build
mkdir Release Debug Profile

cd Debug; cmake -DCMAKE_BUILD_TYPE=Debug  -G "MinGW Makefiles" ../..; cd ..
cd Release; cmake -DCMAKE_BUILD_TYPE=Release  -G "MinGW Makefiles" ../..; cd ..
cd Profile; cmake -DCMAKE_BUILD_TYPE=Profile  -G "MinGW Makefiles" ../..; cd ..