#!/bin/bash

mkdir -p bin

rm -fr CMakeCache.txt
rm -fr CMakeFiles

cmake .
make clean;make

rm -fr CMakeCache.txt
rm -fr CMakeFiles

rm tags
ctags -R --c++-kinds=+px --fields=+iaS --extra=+q .
