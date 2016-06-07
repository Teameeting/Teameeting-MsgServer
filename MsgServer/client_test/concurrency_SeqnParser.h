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


testBaseName="test";
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
    for ((testI=0;testI < testNum; ++testI)); do
        {
            testName=$testBaseName$testI
	    recvName=$testName-recv
	    sendName=$testName-send
            echo "load file name:" $testName
            rest=`$PARSER_PATH/$PARSER_NAME $PARSER_PATH/../$sendName $PARSER_PATH/../$recvName`
            echo "load done..."
        }&
    done
    echo "testClient wait for ending..."
    wait
}

function checkSendAndRecv()
{
    resultName=$CUR_PATH/result.res
    for ((testI=0;testI < testNum; ++testI)); do
        {
            sendName=$testBaseName$testI-send
            recvName=$testBaseName$testI
            if [[ -f "$sendName" ]] && [[ -f "$recvName" ]]; then
                sendLine=`sed -n '$=' $sendName`
                recvLine=`sed -n '$=' $recvName`
                errlog=$testBaseName$testI-log
                if [[ $sendLine -gt 0 ]] && [[ $recvLine -gt 0 ]]; then
                    if [ $sendLine -eq $recvLine ]; then
                        echo "sendLine and recvLine are equal, send:" $sendLine ", recv:" $recvLine
                    else
                        echo "error:sendName:" $sendName " and recvName: " $recvName >> $errlog
                        echo "*****:sendLine:" $sendLine " and recvLine: " $recvLine >> $errlog
                        echo "*****:are not same" >> $errlog
                        cat $errlog >> $resultName
                        testClient
                    fi
                else
                    echo "error:sendName:" $sendName " and recvName: " $recvName >> $errlog
                    echo "#####:sendLine:" $sendLine " and recvLine: " $recvLine >> $errlog
                    echo "#####:are not all greater than 0" >> $errlog
                    cat $errlog >> $resultName
                fi
            else
                echo "error:sendName:" $sendName " and recvName: " $recvName " not exists!"
            fi

        }&
    done
    echo "checkSendAndRecv wait for ending..."
    wait
}

function checkSequenceLine()
{
    for ((testI=0;testI < testNum; ++testI)); do
        {
            sendName=$testBaseName$testI-send
            if [ -f "$sendName" ]; then
                sendLine=`sed -n '$=' $sendName`
                errlog=$testBaseName$testI-log
                if [ $sendLine -gt 0 ]; then
                        echo "sendLine send:" $sendLine
                fi
            else
                echo "error:sendName:" $sendName " not exists!"
            fi

        }&
    done
    echo "checkSequenceLine wait for ending..."
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
rm result
testNum=2;
PARAM_NAME=""
PARAM_PATH=$CUR_PATH
PARSER_NAME="log_parser"
PARSER_PATH=$CUR_PATH/parser
echo "kill PARSER_PATH/PARSER_NAME:" $PARSER_PATH/$PARSER_NAME
#trap "pgrep $PARSER_NAME | xargs kill -s 9;exit 0" INT
#####trap "ps -ef | grep $PARAM_PATH/$PARAM_NAME | grep -v grep | cut -c 9-15 | xargs kill -s 9;exit 0" INT

testClient
