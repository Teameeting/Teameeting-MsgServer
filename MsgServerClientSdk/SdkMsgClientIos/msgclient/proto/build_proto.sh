#!/bin/bash

cp build_proto.sh .build_proto.sh

rm *.h *.cc

protoc common_msg.proto --objc_out=../ -I./
protoc meet_msg.proto --objc_out=../ -I./
protoc meet_msg_type.proto --objc_out=../ -I./

