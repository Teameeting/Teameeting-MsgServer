#!/bin/bash

rm -fr libzkclient.a
rm -fr CMakeCache.txt
rm -fr CMakeFiles
cmake .
rm -fr CMakeCache.txt
rm -fr CMakeFiles
