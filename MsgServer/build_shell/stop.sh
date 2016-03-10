#!/bin/sh

echo "stop meeting..."
./meeting/meeting.sh stop
echo "stop dispatcher..."
sleep 3
./dispatcher/dispatcher.sh stop
echo "stop connector..."
sleep 3
./connector/connector.sh stop
echo "stop ok..."
