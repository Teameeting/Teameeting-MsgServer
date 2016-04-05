#!/bin/sh

i=1
while(($i < 100))
do
valgrind --tool=memcheck --show-reachable=yes --track-fds=yes --leak-check=full --undef-value-errors=yes ./MsgServerMeeting meeting/meeting.conf
sleep 1
done

