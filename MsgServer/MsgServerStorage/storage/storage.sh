#!/bin/bash
# 
# zhengxiaokai@dync.cc
# /etc/rc.d/init.d/MsgServerStorage
# init script for MsgServerStorage processes
#
# processname: MsgServerStorage
# description: MsgServerStorage is a client storage.
# chkconfig: 2345 99 10
#
if [ -f /etc/init.d/functions ]; then
	. /etc/init.d/functions
elif [ -f /etc/rc.d/init.d/functions ]; then
	. /etc/rc.d/init.d/functions
else
	echo -e "\aStorage: unable to locate functions lib. Cannot continue."
	exit -1
fi

ulimit -n 65000
echo 1024 65000 > /proc/sys/net/ipv4/ip_local_port_range

#---------------------------------------------------------------------------
# GLOBAL
#---------------------------------------------------------------------------

RT_HOME=/usr/local/dync/msgserver/storage/bin
RT_CONF=/usr/local/dync/msgserver/storage.conf
RT_LIB=/usr/local/dync/msgserver/storage/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${RT_LIB}
retval=0

#---------------------------------------------------------------------------
# START
#---------------------------------------------------------------------------
Start()
{
	if [ $( pidof -s MsgServerStorage ) ]; then
		echo -n "MsgServerStorage process [${prog}] already running"
		echo_failure
		echo
		return -1;
	fi
		
	if [ ! -x ${RT_HOME}/MsgServerStorage ]; then
		echo -n "MsgServerStorage binary [${prog}] not found."
		echo_failure
		echo
		return -1
	fi
		
	echo -n "Starting Storage Server(MsgServerStorage): "
	${RT_HOME}/MsgServerStorage ${RT_CONF}/storage.conf & 2> /dev/null
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
	echo -n "Stopping Storage Server(MsgServerStorage): "
	killproc MsgServerStorage
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

