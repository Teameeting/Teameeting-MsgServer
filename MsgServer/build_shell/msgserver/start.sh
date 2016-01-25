#!/bin/sh

echo "start connector..."
./connector/connector.sh start
echo "start dispatcher..."
sleep 3
./dispatcher/dispatcher.sh start
sleep 3
echo "start meeting..."
./meeting/meeting.sh start
echo "start ok..."
