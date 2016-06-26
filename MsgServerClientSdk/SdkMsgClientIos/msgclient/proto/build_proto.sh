#!/bin/bash

cp build_proto.sh .build_proto.sh

protoc common_msg.proto       --objc_out=../ -I./
protoc entity_msg.proto       --objc_out=../ -I./
protoc entity_msg_type.proto  --objc_out=../ -I./
protoc sys_msg.proto          --objc_out=../ -I./
protoc sys_msg_type.proto     --objc_out=../ -I./
protoc storage_msg.proto      --objc_out=../ -I./
protoc storage_msg_type.proto --objc_out=../ -I./
