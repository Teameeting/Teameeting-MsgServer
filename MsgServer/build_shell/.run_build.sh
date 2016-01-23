#!/bin/sh

##############################################################
#####################    usage    ############################
##############################################################
function shell_usage()
{
    echo "################## USAGE: $0 [-cfh] [-l log name]"
    sleep 1
    echo "################## USAGE: -c clean"
    sleep 1
    echo "################## USAGE: -f force config"
    sleep 1
    echo "################## USAGE: -l log file name"
    sleep 1
    echo "################## USAGE: -h help usage"
    sleep 1
    exit
}

##############################################################
###################    parameter    ##########################
##############################################################
CUR_PATH=`pwd`
BASE_PATH=$CUR_PATH/..

CUR_DATE=$(date +%Y-%m-%d-%H-%M-%S)

SRC_PATH=
BUILD_NAME=
PREFIX_PATH=
THIRDPARTY_LIB_PATH=

export PARAM_FORCE="no"
export PARAM_CLEAN="no"
export PARAM_LOGNAME=$CUR_DATE

while getopts "c:f:l:h" arg
do
    case $arg in
        c)  PARAM_CLEAN="yes";;
        f)  PARAM_FORCE="yes";;
        l)  PARAM_LOGNAME=$OPTARG;;
        h)  shell_usage $0;;
        ?)  shell_usage $0;;
    esac
done

LOG_FILE=$CUR_PATH/log/log-$PARAM_LOGNAME
touch $LOG_FILE

echo "LOG_FILE: " $LOG_FILE

##############################################################
###################    function    ###########################
##############################################################

###################log to file ###############################
function log2f()
{
	if [ $# -eq 0 ]; then
        echo -e "[LOG]Date: $CUR_DATE" >> $LOG_FILE
	elif [ $# -eq 1 ]; then
        echo -e "[LOG]$CURTIME: $1" >> $LOG_FILE
	elif [ $# -eq 2 ]; then
        echo -e "[LOG]$CURTIME: $1 $2" >> $LOG_FILE
	else
        echo -e "just support two params hahaha~~~"
	fi
}
export function log2f

###################log info ###############################
function loginfo()
{
	if [ $# -eq 0 ]; then
        echo -e "\033[1;34m [INFO]Date: $CUR_DATE \033[0m"
	elif [ $# -eq 1 ]; then
        echo -e "\033[1;34m [INFO]$CUR_DATE: $1 \033[0m"
	elif [ $# -eq 2 ]; then
        echo -e "\033[1;34m [INFO]$CUR_DATE: $1 $2 \033[0m"
	else
        echo -e "\033[1;34m just support two params hahaha~~~ \033[0m"
	fi
}
export function loginfo

###################log error ###############################
function logerr()
{
	if [ $# -eq 0 ]; then
        echo -e "\033[0;31m [ERR]Date: $CUR_DATE \033[0m"
	elif [ $# -eq 1 ]; then
        echo -e "\033[0;31m [ERR]$CUR_DATE: $1 \033[0m"
	elif [ $# -eq 2 ]; then
        echo -e "\033[0;31m [ERR]$CUR_DATE: $1 $2 \033[0m"
	else
        echo -e "\033[1;34m just support two params hahaha~~~ \033[0m"
	fi
}
export function logerr

###################shell_backup ###############################
function shell_backup()
{
  cp $CUR_PATH/run_build.sh $CUR_PATH/.run_build.sh
}

###################start log ###############################
function start_log()
{
	loginfo "##########################  LOG START BUILDING ################################"
}

###################end log ###############################
function end_log()
{
	loginfo "##########################  LOG END BUILDING ################################"
}


###################build third party libs ###############################
###################SRC_PATH ###############################
###################BUILD_NAME ###############################
function build_3rdpartylibs()
{
    if [ $# -eq 2 ]
    then
        if [ -d $1 ]
        then
            cd $1
            ./lib_a.sh
            if [ $? -eq 0 ]
            then
                loginfo "build $2 ok: " $?
                cd $BASE_PATH
            else
                logerr "build $2 err: " $?
                exit
            fi
        else
            logerr "dir $1 not exist..."
        fi
    else
        logerr "params error..."
    fi
}


###################build MSCommon lib ###############################
###################SRC_PATH ###############################
###################PREFIX_PATH ###############################
###################3RD_PARTY_PATH ###############################
###################BUILD_NAME ###############################
function build_mscommonlib()
{
    if [ $# -eq 4 ]
    then
        if [ -d $1 ] && [ -d $2 ]
        then
            cd $1
            if [ "$PARAM_FORCE"x = "yes"x ]
            then
                autoreconf -ivf
                ./configure --prefix=$2
                make clean
            fi
            if [ "$PARAM_CLEAN"x = "yes"x ]
            then
                make clean
            fi
            make && make install
            if [ $? -eq 0 ]
            then
                cp $3/*.a $2/
                loginfo "build $4 ok: " $?
                cd $BASE_PATH
            else
                logerr "build $4 err: " $?
                exit
            fi
        else
            logerr "dir $1 not exist..."
        fi
    else
        logerr "params error..."
    fi
}


###################build bin ###############################
###################SRC_PATH ###############################
###################PREFIX_PATH ###############################
###################BIN_NAME ###############################
function build_bin()
{
    if [ $# -eq 3 ]
    then
        if [ -d $1 ] && [ -d $2 ]
        then
            cd $1
            if [ "$PARAM_FORCE"x = "yes"x ]
            then
                autoreconf -ivf
                ./configure --prefix=$2
                make clean
            fi
            if [ "$PARAM_CLEAN"x = "yes"x ]
            then
                make clean
            fi
            make && make install
            if [ $? -eq 0 ]
            then
                loginfo "build $3 ok: " $?
                cd $BASE_PATH
            else
                logerr "build $3 err: " $?
                exit
            fi
        else
            logerr "dir $1 not exist..."
        fi
    else
        logerr "params error..."
    fi
}

##############################################################
####################    program    ###########################
##############################################################
shell_backup
start_log
sleep 2

####################    building 3rdpartylibs    ###########################
SRC_PATH=$BASE_PATH/MsgServer/MSCommon
BUILD_NAME=ThirdParybLibs
build_3rdpartylibs $SRC_PATH $BUILD_NAME

####################    building mscommonlib    ###########################
SRC_PATH=$BASE_PATH/MsgServer
BUILD_NAME=MsgCommonLib
PREFIX_PATH=$BASE_PATH/MsgServer/MSCommonLib
THIRDPARTY_LIB_PATH=$BASE_PATH/MsgServer/MSCommon/lib_linux_a
build_mscommonlib $SRC_PATH $PREFIX_PATH $THIRDPARTY_LIB_PATH $BUILD_NAME

####################    building msgserverconnector    ###########################
SRC_PATH=$BASE_PATH/MsgServerConnector
BUILD_NAME=MsgServerConnector
PREFIX_PATH=$SRC_PATH/connector
build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME

####################    building msgserverdispatcher    ###########################
SRC_PATH=$BASE_PATH/MsgServerDispatcher
BUILD_NAME=MsgServerDispatcher
PREFIX_PATH=$SRC_PATH/dispatcher
build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME

####################    building msgservermeeting    ###########################
SRC_PATH=$BASE_PATH/MsgServerMeeting
BUILD_NAME=MsgServerMeeting
PREFIX_PATH=$SRC_PATH/meeting
build_bin $SRC_PATH $PREFIX_PATH $BUILD_NAME

end_log
loginfo "build all bin successfully..."
sleep 1
loginfo "build all bin successfully..."
sleep 1
loginfo "build all bin successfully..."
sleep 1
exit
