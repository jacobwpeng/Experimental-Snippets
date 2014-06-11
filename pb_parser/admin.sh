#!/bin/bash - 
#===============================================================================
#
#          FILE: admin.sh
# 
#         USAGE: ./admin.sh [make||rebuildclean|start|stop]
# 
#   DESCRIPTION: 
# 
#===============================================================================

set -o nounset                              # Treat unset variables as an error

function Usage
{
    echo "$0 [make|rebuild|clean|start|stop]"
    exit -1
}

function DoMake
{
    cd build/; make -j4
}

function DoRebuild
{
    mkdir -p build/ && cd build/ && cmake .. -DCMAKE_BUILD_TYPE=debug && make -j4
}

#function DoClean
#{
#    #no-op
#}
#
function DoRun
{
    ./build/pbparser /tmp/pair.desc /tmp/pair.dump
}
#
#function DoStop
#{
#    #killall logsvrd
#}

[[ $# -ne 1 ]] && Usage

cmd=$1

case "$cmd" in
    "start")
        DoRun
    ;;
    "stop")
        DoStop
    ;;
    "make")
        DoMake
    ;;
    "rebuild")
        DoRebuild
    ;;
    "clean")
        DoClean
    ;;
    *)
        Usage
    ;;
esac


