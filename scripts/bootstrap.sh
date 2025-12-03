#!/bin/sh

cmake -S . -B bootstrap -DCMAKE_BUILD_TYPE=Release
cmake --build bootstrap

rm -rf bootstrap
