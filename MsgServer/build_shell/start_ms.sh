#!/bin/sh

echo "start msgserver begin ok..."

echo "start sequence..."
./sequence/sequence.sh start
sleep 3

echo "start storage..."
./storage/storage.sh start
sleep 3

echo "start logical..."
./logical/logical.sh start
sleep 3

echo "start connector..."
./connector/connector.sh start
sleep 3

echo "start dispatcher..."
./dispatcher/dispatcher.sh start
sleep 3

echo "start rtlive..."
./rtlive/rtlive.sh start
sleep 3

echo "start grouper..."
./grouper/grouper.sh start
sleep 3

echo "start msgserver finished ok..."
