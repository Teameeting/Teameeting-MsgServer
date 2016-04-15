#!/bin/sh

##############################################################
###################    parameter    ##########################
##############################################################
LIB_CUR_PATH=`pwd`
if [ ! -d lib_linux_a ]
then
    mkdir -p lib_linux_a
fi
LIB_LINUX_PATH=$LIB_CUR_PATH/lib_linux_a
LIB_BASE_PATH=$LIB_CUR_PATH/MSThirdParty
echo  "lib cur   path:" $LIB_CUR_PATH
echo  "lib linux path:" $LIB_LINUX_PATH
echo  "lib base  path:" $LIB_BASE_PATH

LIB_SRC_PATH=
LIB_NAME=


##############################################################
###################    function    ###########################
##############################################################

###################clean up all libs #########################
function cleanup()
{
    if [ "$PARAM_CLEAN"x = "yes"x ]
    then
        loginfo "[]rm all *.a"
        if [ -d $LIB_LINUX_PATH ]
        then
            rm $LIB_LINUX_PATH/*
        else
            loginfo "dir $LIB_LINUX_PATH not exist..."
            return 1
        fi
    fi
}

###################build all libs #########################
###################LIB_SRC_PATH ###########################
###################LIB_NAME ###############################
function build_lib()
{
    cd $1
    loginfo "[]generic $2..."
    if [ "$PARAM_CLEAN"x = "yes"x ]
    then
        make clean
    fi
    make
    if [ $? -eq 0 ]
    then
        cp $1/$2 $LIB_LINUX_PATH/
        loginfo "build $2 ok"
        sleep 1
        cd $LIB_BASE_PATH
    else
        loginfo "build $2 err: " $?
        return 1
    fi
}

##############################################################
####################    program    ###########################
##############################################################

loginfo
####################    cleanning all libs  ##########################
cleanup

####################    building libhiredis  ##########################
LIB_SRC_PATH=$LIB_BASE_PATH/hiredis
LIB_NAME=libhiredis.a
build_lib $LIB_SRC_PATH $LIB_NAME
if [ $? -ne 0 ]
then
    logerr "build_lib $LIB_NAME error..."
    exit 2
fi
sleep 1

####################    building libhttp  ##########################
LIB_SRC_PATH=$LIB_BASE_PATH/libhttp
LIB_NAME=libhttp.a
build_lib $LIB_SRC_PATH $LIB_NAME
if [ $? -ne 0 ]
then
    logerr "build_lib $LIB_NAME error..."
    exit 2
fi
sleep 1

####################    building libRtComm  ##########################
LIB_SRC_PATH=$LIB_BASE_PATH/RTComm
LIB_NAME=libRtComm.a
build_lib $LIB_SRC_PATH $LIB_NAME
if [ $? -ne 0 ]
then
    logerr "build_lib $LIB_NAME error..."
    exit 2
fi
sleep 1

####################    building librtklog  ##########################
LIB_SRC_PATH=$LIB_BASE_PATH/rtklog
LIB_NAME=librtklog.a
build_lib $LIB_SRC_PATH $LIB_NAME
if [ $? -ne 0 ]
then
    logerr "build_lib $LIB_NAME error..."
    exit 2
fi
sleep 1

####################    building libzkclient  ##########################
LIB_SRC_PATH=$LIB_BASE_PATH/zkclient
LIB_NAME=libzkclient.a
cd $LIB_SRC_PATH
sh build.sh
build_lib $LIB_SRC_PATH $LIB_NAME
if [ $? -ne 0 ]
then
    logerr "build_lib $LIB_NAME error..."
    exit 2
fi
sleep 1

echo "[]generic lib.a ok"
sleep 1
echo "[]generic lib.a ok"
sleep 1
loginfo
