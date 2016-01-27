#!/bin/sh
CUR_PATH=`pwd`
echo "cur_path:" $CUR_PATH

CLEAN_FLAG=0
if [ "$1"x = "-u"x ]
then
    CLEAN_FLAG=1
fi

#######################  install module  #########################################
#######################  $1 module dir  #########################################
function install_module()
{
    if [ $# -eq 1 ]
    then
        if [ -d $1 ]
        then
            cd $1
            ./install.sh
            if [ $? -eq 0 ]
            then
                echo "install $1 ok"
            else
                echo "install $1 error"
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
function uninstall_module()
{
    if [ $# -eq 1 ]
    then
        if [ -d $1 ]
        then
            cd $1
            ./install.sh -u
            if [ $? -eq 0 ]
            then
                echo "uninstall $1 ok"
            else
                echo "uninstall $1 error"
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
     
}

####find . -name "*.Po" | xargs rm -rf
####find . -name "*.bz2" | xargs rm -rf
