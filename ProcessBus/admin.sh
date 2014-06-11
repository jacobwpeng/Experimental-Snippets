#!/bin/bash - 
#===============================================================================
#
#          FILE: admin.sh
# 
#         USAGE: ./admin.sh [make|clean]
# 
#   DESCRIPTION: 
# 
#===============================================================================

set -o nounset                              # Treat unset variables as an error

function Usage
{
    echo "$0 [make|rebuild|clean|start]"
    exit -1
}

function DoMake
{
    make -j4 -C build/
}

function DoRebuild
{
    mkdir -p build/ && cd build/ && cmake .. -DCMAKE_BUILD_TYPE=release && make -j4
}

function DoStart
{
    ./build/logclient ../logsvrd/conf/logsvrd.xml
}

[[ $# -ne 1 ]] && Usage

cmd=$1

case "$cmd" in
    "make")
        DoMake
    ;;
    "rebuild")
        DoRebuild
    ;;
    "clean")
        DoClean
    ;;
    "start")
        DoStart
    ;;
    *)
        Usage
    ;;
esac


