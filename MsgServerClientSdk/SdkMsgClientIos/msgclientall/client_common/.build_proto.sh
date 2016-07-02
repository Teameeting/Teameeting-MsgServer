#!/bin/bash

cp build_proto.sh .build_proto.sh

rm proto/common_msg.pb.cc         
rm proto/common_msg.pb.h          
rm proto/entity_msg.pb.cc         
rm proto/entity_msg.pb.h          
rm proto/entity_msg_type.pb.cc
rm proto/entity_msg_type.pb.h 
rm proto/sys_msg.pb.cc
rm proto/sys_msg.pb.h
rm proto/sys_msg_type.pb.cc
rm proto/sys_msg_type.pb.h 
rm proto/storage_msg.pb.cc    
rm proto/storage_msg.pb.h     
rm proto/storage_msg_type.pb.cc    
rm proto/storage_msg_type.pb.h     


protoc proto/common_msg.proto       --cpp_out=./proto -I./proto
protoc proto/entity_msg.proto       --cpp_out=./proto -I./proto
protoc proto/entity_msg_type.proto  --cpp_out=./proto -I./proto
protoc proto/sys_msg.proto          --cpp_out=./proto -I./proto
protoc proto/sys_msg_type.proto     --cpp_out=./proto -I./proto
protoc proto/storage_msg.proto      --cpp_out=./proto -I./proto
protoc proto/storage_msg_type.proto --cpp_out=./proto -I./proto
