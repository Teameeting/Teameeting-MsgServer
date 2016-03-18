#!/bin/bash

rm -fr libzkclient.a
rm -fr CMakeCache.txt
rm -fr CMakeFiles
cmake .
make clean
make
rm -fr CMakeCache.txt
rm -fr CMakeFiles
