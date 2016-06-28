#!/bin/bash

cp build_proto.sh .build_proto.sh

protoc common_msg.proto     --java_out=../../../../../ --proto_path=./
protoc meet_msg.proto       --java_out=../../../../../ --proto_path=./
protoc meet_msg_type.proto  --java_out=../../../../../ --proto_path=./

