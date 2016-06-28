#!/bin/bash

cp build_proto.sh .build_proto.sh

protoc proto/common_msg.proto       --cpp_out=./proto -I./proto
protoc proto/entity_msg.proto       --cpp_out=./proto -I./proto
protoc proto/entity_msg_type.proto  --cpp_out=./proto -I./proto
protoc proto/sys_msg.proto          --cpp_out=./proto -I./proto
protoc proto/sys_msg_type.proto     --cpp_out=./proto -I./proto
protoc proto/storage_msg.proto      --cpp_out=./proto -I./proto
protoc proto/storage_msg_type.proto --cpp_out=./proto -I./proto
