#!/bin/sh

echo "tar begin *_*..."

echo "tar connector.tar.gz..."
sleep 1
tar -zcvf connector.tar.gz connector

echo "tar dispatcher.tar.gz..."
sleep 1
tar -zcvf dispatcher.tar.gz dispatcher

echo "tar meeting.tar.gz..."
sleep 1
tar -zcvf meeting.tar.gz meeting

echo "tar rtlive.tar.gz..."
sleep 1
tar -zcvf rtlive.tar.gz rtlive

echo "tar logical.tar.gz..."
sleep 1
tar -zcvf logical.tar.gz logical

echo "tar sequence.tar.gz..."
sleep 1
tar -zcvf sequence.tar.gz sequence

echo "tar storage.tar.gz..."
sleep 1
tar -zcvf storage.tar.gz storage

echo "tar grouper.tar.gz..."
sleep 1
tar -zcvf grouper.tar.gz grouper

echo "tar pusher.tar.gz..."
sleep 1
tar -zcvf pusher.tar.gz pusher

echo "tar end *_*..."
