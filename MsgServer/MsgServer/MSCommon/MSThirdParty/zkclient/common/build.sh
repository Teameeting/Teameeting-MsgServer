#!/bin/bash


mkdir -p lib
rm -fr CMakeCache.txt
rm -fr CMakeFiles
cmake .
make clean;make
rm -fr CMakeCache.txt
rm -fr CMakeFiles
