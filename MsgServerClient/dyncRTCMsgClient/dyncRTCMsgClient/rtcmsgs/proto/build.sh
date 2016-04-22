#!/bin/bash

rm msg_type.pb.*
rm meet_msg.pb.*
rm sys_msg.pb.*

protoc msg_type.proto --cpp_out=./ -I./
protoc meet_msg.proto --cpp_out=./ -I./
protoc sys_msg.proto --cpp_out=./ -I./

