#!/bin/bash

rm -fr libzkclient.a
rm -fr CMakeCache.txt
rm -fr CMakeFiles
cmake .
make clean
rm -fr CMakeCache.txt
rm -fr CMakeFiles
