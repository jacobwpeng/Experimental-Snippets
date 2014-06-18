#!/bin/bash - 
#===============================================================================
#
#          FILE: admin.sh
# 
#         USAGE: ./admin.sh [make|rebuild|clean|start|stop|test]
# 
#   DESCRIPTION: 
# 
#===============================================================================

set -o nounset                              # Treat unset variables as an error

function Usage
{
    echo "$0 [make|rebuild|clean|start|stop|test]"
    exit -1
}

function DoMake
{
    cd build/; make -j4
}

function DoRebuild
{
    mkdir -p build/ && cd build/ && cmake .. -DCMAKE_BUILD_TYPE=release && make VERBOSE=1 -j4
}

#function DoClean
#{
#}
#
function DoRun
{
    ./build/example/run data/userinfo.desc data/userinfo.backup
#    ./build/run /tmp/pair.desc /tmp/pair.dump
}
function DoTest
{
    ./build/tests/gtest-all
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
    "test")
        DoTest
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


