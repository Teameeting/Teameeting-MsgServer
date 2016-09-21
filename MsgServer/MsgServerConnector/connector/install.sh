#!/bin/sh
CLEAN_FLAG=0
if [ "$1" = "-u" ]; then
CLEAN_FLAG=1
fi

TARGET_PREFIX=/usr/local/dync/msgserver/connector
TARGET_CONF_PATH=$TARGET_PREFIX/conf
TARGET_BIN_PATH=$TARGET_PREFIX/bin
TARGET_LOG_PATH=$TARGET_PREFIX/log
TARGET_LIB_PATH=$TARGET_PREFIX/lib
SYS_BIN_PATH=/usr/bin
SYS_INIT_PATH=/etc/init.d

if [ "$CLEAN_FLAG" = "0" ]; then
printf "Install program....\r\n"
mkdir -p $TARGET_CONF_PATH
mkdir -p $TARGET_BIN_PATH
mkdir -p $TARGET_LOG_PATH
mkdir -p $TARGET_LIB_PATH
mkdir -p $SYS_BIN_PATH
# exe sh
cp -f connector.sh $SYS_BIN_PATH/connector.sh
chmod 755 $SYS_BIN_PATH/connector.sh

cp -f connector.conf $TARGET_CONF_PATH/
cp -f params.conf $TARGET_CONF_PATH/
cp -f bin/MsgServerConnector $TARGET_BIN_PATH/
printf "Install program ok!\r\n"
else
printf "UnInstall program....\r\n"
connector.sh stop
rm -f $SYS_BIN_PATH/connector.sh
rm -f $TARGET_CONF_PATH/connector.conf
rm -f $TARGET_CONF_PATH/params.conf
rm -f $TARGET_BIN_PATH/MsgServerConnector
rm -rf $TARGET_PREFIX
printf "UnInstall program ok!\r\n"
fi
