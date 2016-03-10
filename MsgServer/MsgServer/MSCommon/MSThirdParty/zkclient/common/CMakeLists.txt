cmake_minimum_required(VERSION 2.6)
project(common)
set(CMAKE_VERBOSE_MAKEFILE ON)
add_definitions("-Wall -g")
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/../efnfw include /usr/local/Cellar/jsoncpp/0.10.5/include /usr/local/include)
AUX_SOURCE_DIRECTORY(src SRC_LIST)
add_library(common STATIC ${SRC_LIST})
