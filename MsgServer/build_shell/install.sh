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
    echo "$1 is:" $1
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

#######################  install teameeting  #########################################
function install_teameeting()
{
    cd $CUR_PATH
    MODULE_DIR=connector
    MODULE_NAME="connector"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=dispatcher
    MODULE_NAME="dispatcher"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=meeting
    MODULE_NAME="meeting"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1
}


#######################  uninstall teameeting  #########################################
function uninstall_teameeting()
{
    cd $CUR_PATH
    MODULE_DIR=connector
    MODULE_NAME="connector"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=dispatcher
    MODULE_NAME="dispatcher"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=meeting
    MODULE_NAME="meeting"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1
}

#######################  install msgserver  #########################################
function install_msgserver()
{
    cd $CUR_PATH
    MODULE_DIR=connector
    MODULE_NAME="connector"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=dispatcher
    MODULE_NAME="dispatcher"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=rtlive
    MODULE_NAME="rtlive"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=logical
    MODULE_NAME="logical"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=sequence
    MODULE_NAME="sequence"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=storage
    MODULE_NAME="storage"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=grouper
    MODULE_NAME="grouper"
    echo "module_dir:" $MODULE_DIR
    install_module $MODULE_DIR $MODULE_NAME
    sleep 1
}


#######################  uninstall msgserver  #########################################
function uninstall_msgserver()
{
    cd $CUR_PATH
    MODULE_DIR=connector
    MODULE_NAME="connector"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=dispatcher
    MODULE_NAME="dispatcher"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=rtlive
    MODULE_NAME="rtlive"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=logical
    MODULE_NAME="logical"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=sequence
    MODULE_NAME="sequence"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=storage
    MODULE_NAME="storage"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1

    cd $CUR_PATH
    MODULE_DIR=grouper
    MODULE_NAME="grouper"
    uninstall_module $MODULE_DIR $MODULE_NAME
    sleep 1
}

#######################  teameeting usage #########################################
function usage()
{
    echo "######### teameeting ########"
    echo "######### teameeting [-iuh]"
    echo "######### -i  install teameeting"
    echo "######### -u  uninstall teameeting"
    echo "######### -j  install msgserver"
    echo "######### -v  uninstall msgserver"
    echo "######### -h  usage: $0 [-iuh]"
    sleep 2
    exit
}

if [ $# -eq 0 ]
then
    usage
    exit
fi

while getopts "iujvh" arg
do
    case $arg in
        i)  install_teameeting ;;
        u)  uninstall_teameeting ;;
        j)  install_msgserver ;;
        v)  uninstall_msgserver ;;
        h)  usage ;;
        ?)  usage ;;
    esac
done

