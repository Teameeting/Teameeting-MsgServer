#!/bin/sh

##############################################################
#####################    usage    ############################
##############################################################
function shell_usage()
{
    echo "################## USAGE: $0 [-fph]"
    sleep 1
    echo "################## USAGE: -f [yes/no] force config"
    sleep 1
    echo "################## USAGE: -p [path name] client bin path name"
    sleep 1
    echo "################## USAGE: -h help usage"
    sleep 1
    exit
}

##############################################################
###################    parameter    ##########################
##############################################################
CUR_PATH=`pwd`
CUR_DATE=$(date +%Y-%m-%d-%H-%M-%S)

PARAM_FORCE="no"
PARAM_PATH="."
PARAM_NAME="demo"

while getopts "fp:h" arg
do
    case $arg in
        f)  PARAM_FORCE="yes";;
        p)  PARAM_NAME=$OPTARG;;
        h)  shell_usage $0;;
        ?)  shell_usage $0;;
    esac
done

echo "param_force:" $PARAM_FORCE
echo "param path:"  $PARAM_PATH
echo "param_name:"  $PARAM_NAME


testBaseName="TestUserId0";
testNum=3;
testI=0;

###########################################
###############function####################
###########################################
function testDemo()
{
    for ((testI=0;testI < testNum; ++testI)); do
        echo "hello:" $testI
        testName=$testBaseName$testI
        echo $testName
        {
            if [ "$PARAM_NAME"x != ""x ]
            then
                echo "load bin..." $PARAM_NAME
                ##rest=`$PARAM_NAME $testName`
                ##echo $rest
                `$PARAM_PATH/$PARAM_NAME $testName &`
            fi
        }&
    done
    echo "wait for ending..."
    wait
}

function testClient()
{
    for ((testI=1;testI <= testNum; ++testI)); do
        {
            echo "hello:" $testI
            testName=$testBaseName$testI
            echo "load client:" $PARAM_PATH/$PARAM_NAME ",user:" $testName
            rest=`$PARAM_PATH/$PARAM_NAME $testName`
            echo "load done..."
        }&
    done
    echo "wait for ending..."
    wait
}

###########################################
#################test######################
###########################################

#################################################
##testNum=3;
##PARAM_NAME="demo"
##PARAM_PATH="."
##trap "pgrep $PARAM_NAME | xargs kill -s 9;exit 0" INT
##testDemo

#################################################
rm test*
testNum=20;
PARAM_NAME="MsgServerClient"
PARAM_PATH=$CUR_PATH/../MsgServerClient
echo "kill PARAM_PATH/PARAM_NAME:" $PARAM_PATH/$PARAM_NAME
trap "pgrep $PARAM_NAME | xargs kill -s 9;exit 0" INT
##trap "ps -ef | grep $PARAM_PATH/$PARAM_NAME | grep -v grep | cut -c 9-15 | xargs kill -s 9;exit 0" INT
testClient

