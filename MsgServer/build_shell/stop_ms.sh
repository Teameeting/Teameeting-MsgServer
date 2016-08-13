#!/bin/sh

echo "stop msgserver begin ok..."

echo "stop grouper.."
./grouper/grouper.sh stop
sleep 3

echo "stop rtlive..."
./rtlive/rtlive.sh stop
sleep 3

echo "stop dispatcher..."
./dispatcher/dispatcher.sh stop
sleep 3

echo "stop connector..."
./connector/connector.sh stop
sleep 3

echo "stop logical..."
./logical/logical.sh stop
sleep 3

echo "stop storage..."
./storage/storage.sh stop
sleep 3

echo "stop sequence..."
./sequence/sequence.sh stop
sleep 3

echo "stop msgserver finished ok..."
