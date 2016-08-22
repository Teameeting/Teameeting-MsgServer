#!/bin/sh

echo "untar begin *_*..."

echo "untar connector.tar.gz..."
sleep 1
tar -zxvf connector.tar.gz

echo "untar dispatcher.tar.gz..."
sleep 1
tar -zxvf dispatcher.tar.gz

echo "untar meeting.tar.gz..."
sleep 1
tar -zxvf meeting.tar.gz

echo "untar rtlive.tar.gz..."
sleep 1
tar -zxvf rtlive.tar.gz

echo "untar logical.tar.gz..."
sleep 1
tar -zxvf logical.tar.gz

echo "untar sequence.tar.gz..."
sleep 1
tar -zxvf sequence.tar.gz

echo "untar storage.tar.gz..."
sleep 1
tar -zxvf storage.tar.gz

echo "untar grouper.tar.gz..."
sleep 1
tar -zxvf grouper.tar.gz

echo "untar pusher.tar.gz..."
sleep 1
tar -zxvf pusher.tar.gz

echo "untar end *_*..."
