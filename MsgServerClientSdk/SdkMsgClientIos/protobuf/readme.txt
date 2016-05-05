in order to compile android libprotobuf-lite.a
I remove the followint files and dirs:
.libs
.deps
.dirstamp
.o
unittest
test
.lo
.proto
.pb.* (except descriptor.pb.cc)
compiler
arena_nc.cc
util

