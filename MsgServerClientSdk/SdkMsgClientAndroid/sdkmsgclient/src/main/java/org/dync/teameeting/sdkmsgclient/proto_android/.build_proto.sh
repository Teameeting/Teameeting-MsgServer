#!/bin/bash

cp build_proto.sh .build_proto.sh

protoc common_msg.proto					--java_out=../../../../../ --proto_path=./
protoc entity_msg.proto					--java_out=../../../../../ --proto_path=./
protoc entity_msg_type.proto		--java_out=../../../../../ --proto_path=./
protoc sys_msg.proto						--java_out=../../../../../ --proto_path=./
protoc sys_msg_type.proto				--java_out=../../../../../ --proto_path=./
protoc storage_msg.proto				--java_out=../../../../../ --proto_path=./
protoc storage_msg_type.proto		--java_out=../../../../../ --proto_path=./

