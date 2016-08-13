#!/bin/sh

##sed -i '/^debug/{s/^.*$/debug=0/}' params.conf
##sed -n '/^debug/p' params.conf

function shell_usage()
{
    echo "################## USAGE: $0 [-drh]"
    sleep 1
    echo "################## USAGE: -d debug"
    sleep 1
    echo "################## USAGE: -r release"
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

export BUILD_TYPE="debug"

while getopts "drh" arg
do
    case $arg in
        d)  BUILD_TYPE="debug";;
        r)  BUILD_TYPE="release";;
        h)  shell_usage $0;;
        ?)  shell_usage $0;;
    esac
done

echo "build_type is :" $BUILD_TYPE

function echo_build_type()
{
    ret=`find ./ -iname "params.conf" | xargs sed -n '/^debug/p'`
    echo $ret
}

function build_4_debug()
{
    ##find ./ -iname "$1"
    res=`find ./ -iname "$1"`
    ##echo $res
    for it in $res
    do
        echo $it
        sed -i '/^debug/{s/^.*$/debug='$2'/}' $it
        sed -n '/^debug/p' $it
    done
}

function do_build()
{
    if [ "$BUILD_TYPE"x = "debug"x ]
    then
        build_4_debug "params.conf" 1
    else
        build_4_debug "params.conf" 0
    fi
}

do_build
echo ""
echo ""
echo ""
echo "*_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_*"
echo_build_type
echo "*_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_*"
echo ""
echo ""
echo ""
