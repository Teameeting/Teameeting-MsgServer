#!/bin/bash
# 
# zhengxiaokai@dync.cc
# /etc/rc.d/init.d/MsgServerRTRedis
# init script for MsgServerRTRedis processes
#
# processname: MsgServerRTRedis
# description: MsgServerRTRedis is a client rtredis.
# chkconfig: 2345 99 10
#
if [ -f /etc/init.d/functions ]; then
	. /etc/init.d/functions
elif [ -f /etc/rc.d/init.d/functions ]; then
	. /etc/rc.d/init.d/functions
else
	echo -e "\artredis: unable to locate functions lib. Cannot continue."
	exit -1
fi

ulimit -n 65000
echo 1024 65000 > /proc/sys/net/ipv4/ip_local_port_range

#---------------------------------------------------------------------------
# GLOBAL
#---------------------------------------------------------------------------

RT_HOME=/usr/local/dync/msgserver/rtredis/bin
RT_CONF=/usr/local/dync/msgserver/rtredis/conf
RT_LIB=/usr/local/dync/msgserver/rtredis/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${RT_LIB}
retval=0

#---------------------------------------------------------------------------
# START
#---------------------------------------------------------------------------
Start()
{
	if [ $( pidof -s MsgServerRTRedis ) ]; then
		echo -n "MsgServerRTRedis process [${prog}] already running"
		echo_failure
		echo
		return -1;
	fi
		
	if [ ! -x ${RT_HOME}/MsgServerRTRedis ]; then
		echo -n "MsgServerRTRedis binary [${prog}] not found."
		echo_failure
		echo
		return -1
	fi
		
	echo -n "Starting rtredis Server(MsgServerRTRedis): "
	${RT_HOME}/MsgServerRTRedis ${RT_CONF}/rtredis.conf & 2> /dev/null
	retval=$?
	if [ ${retval} == 0 ]; then
		echo_success
		echo
	else
		echo_failure
		echo
		break
	fi
	sleep 1
	
	return 0
}

#---------------------------------------------------------------------------
# STOP
#---------------------------------------------------------------------------
Stop()
{
	echo -n "Stopping rtredis Server(MsgServerRTRedis): "
	killproc MsgServerRTRedis
	echo
	return 0
}

#---------------------------------------------------------------------------
# MAIN
#---------------------------------------------------------------------------
case "$1" in
	start)
		Start
		;;
	stop)
		Stop
		;;
	restart)
		Stop
		sleep 5
		Start
		;;
	*)
		echo "Usage: $0 {start|stop|restart}"
esac

exit 0

