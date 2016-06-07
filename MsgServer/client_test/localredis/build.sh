#!/bin/bash

mkdir -p bin

rm -fr CMakeCache.txt
rm -fr CMakeFiles

cmake .
make clean;make

rm -fr CMakeCache.txt
rm -fr CMakeFiles

rm -rf CMakeCache.txt  CMakeFiles  cmake_install.cmake

rm tags
ctags -R --c++-kinds=+px --fields=+iaS --extra=+q .
