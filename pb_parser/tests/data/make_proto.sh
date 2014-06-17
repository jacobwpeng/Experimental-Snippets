#!/bin/bash - 
#===============================================================================
#
#          FILE: make_proto.sh
# 
#         USAGE: ./make_proto.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dr. Fritz Mehner (fgm), mehner.fritz@fh-swf.de
#  ORGANIZATION: FH S¨¹dwestfalen, Iserlohn, Germany
#       CREATED: 06/10/14 14:24
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
protoc -I=. --cpp_out=. TestPacked.proto
protoc -o TestPacked.desc TestPacked.proto

