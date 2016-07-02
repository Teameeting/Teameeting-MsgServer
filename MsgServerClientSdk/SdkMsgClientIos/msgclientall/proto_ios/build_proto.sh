#!/bin/bash

cp build_proto.sh .build_proto.sh

protoc common_msg.proto       --objc_out=./ -I./
protoc entity_msg.proto       --objc_out=./ -I./
protoc entity_msg_type.proto  --objc_out=./ -I./
protoc sys_msg.proto          --objc_out=./ -I./
protoc sys_msg_type.proto     --objc_out=./ -I./
protoc storage_msg.proto      --objc_out=./ -I./
protoc storage_msg_type.proto --objc_out=./ -I./

mv CommonMsg.pbobjc.m				CommonMsg.pbobjc.mm
mv EntityMsg.pbobjc.m				EntityMsg.pbobjc.mm
mv EntityMsgType.pbobjc.m		EntityMsgType.pbobjc.mm
mv StorageMsg.pbobjc.m			StorageMsg.pbobjc.mm
mv StorageMsgType.pbobjc.m	StorageMsgType.pbobjc.mm
mv SysMsgType.pbobjc.m			SysMsgType.pbobjc.mm
mv SysMsg.pbobjc.m					SysMsg.pbobjc.mm
