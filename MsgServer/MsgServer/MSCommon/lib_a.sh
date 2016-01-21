#!/bin/sh

echo "[]rm all *.a"
rm lib_a/*

echo "[]generic libhiredis.a..."
cd MSThirdParty/hiredis/
make clean && make
if [ $? -eq 0 ]
then
    cp libhiredis.a ../../lib_a/
    echo "[]generic libhiredis.a ok: " $?
else
    echo "[]generic libhiredis.a error: " $?
    exit
fi
cd ../../

echo "[]generic libhttp.a..."
cd MSThirdParty/libhttp/
make clean && make
if [ $? -eq 0 ]
then
    cp libhttp.a ../../lib_a/
    echo "[]generic libhttp.a ok: " $?
else
    echo "[]generic libhttp.a error: " $?
    exit
fi
cd ../../

echo "[]generic libRtComm.a..."
cd MSThirdParty/RTComm/
make clean && make
if [ $? -eq 0 ]
then
    cp libRtComm.a ../../lib_a/
    echo "[]generic libRtComm.a ok: " $?
else
    echo "[]generic libRtComm.a error: " $?
    exit
fi
cd ../../

echo "[]generic librtklog.a..."
cd MSThirdParty/rtklog/
make clean && make
if [ $? -eq 0 ]
then
    cp librtklog.a ../../lib_a/
    echo "[]generic librtklog.a ok: " $?
else
    echo "[]generic librtklog.a error: " $?
    exit
fi
cd ../../

echo "[]generic lib.a ok"
