#!/bin/sh
CUR_PATH=`pwd`
echo "cur_path:" $CUR_PATH

MODULE_DIR=""
MODULE_NAME=""
CLEAN_FLAG=0
if [ "$1"x = "-u"x ]
then
    CLEAN_FLAG=1
fi

#######################  install module  #########################################
#######################  $1 module dir  #########################################
#######################  $2 module name  #########################################
function install_module()
{
    if [ $# -eq 2 ]
    then
        if [ -d $1 ]
        then
            cd $1
            ./install.sh
            if [ $? -eq 0 ]
            then
                echo "install $2 ok"
            else
                echo "install $2 error"
                exit
            fi
        else
            echo "dir $1 not exist"
            exit
        fi
    else
        echo "params error"
        exit
    fi
}


#######################  uninstall module  #########################################
#######################  $1 module dir  #########################################
#######################  $1 module name  #########################################
function uninstall_module()
{
    if [ $# -eq 2 ]
    then
        if [ -d $1 ]
        then
            cd $1
            ./install.sh -u
            if [ $? -eq 0 ]
            then
                echo "uninstall $2 ok"
            else
                echo "uninstall $2 error"
                exit
            fi
        else
            echo "dir $1 not exist"
            exit
        fi
    else
        echo "params error"
        exit
    fi
}

function install_teameeting()
{
    MODULE_DIR=$CUR_PATH/connector
    MODULE_NAME="connector"
    install $MODULE_DIR $MODULE_NAME
    MODULE_DIR=$CUR_PATH/dispatcher
    MODULE_NAME="dispatcher"
    install $MODULE_DIR $MODULE_NAME
    MODULE_DIR=$CUR_PATH/meeting
    MODULE_NAME="meeting"
    install $MODULE_DIR $MODULE_NAME
}


function uninstall_teameeting()
{
    MODULE_DIR=$CUR_PATH/connector
    MODULE_NAME="connector"
    uninstall $MODULE_DIR $MODULE_NAME
    MODULE_DIR=$CUR_PATH/dispatcher
    MODULE_NAME="dispatcher"
    uninstall $MODULE_DIR $MODULE_NAME
    MODULE_DIR=$CUR_PATH/meeting
    MODULE_NAME="meeting"
    uninstall $MODULE_DIR $MODULE_NAME
}

function usage()
{
    echo "######### teameeting ########"
    echo "######### teameeting [-iuh]"
    echo "######### -i  install"
    echo "######### -u  uninstall"
    echo "######### -h  usage: $0 [-iuh]"
    sleep 2
    exit
}

function main()
{
    while getopts "i:u:h" arg
    do
        case $arg in
            i)  install_teameeting ;;
            u)  uninstall_teameeting ;;
            h)  usage ;;
            ?)  usage ;;
        esac
    done
}
