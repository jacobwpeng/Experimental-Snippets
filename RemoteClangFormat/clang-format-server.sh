#!/bin/bash - 
#===============================================================================
#
#          FILE: clang-format-server.sh
# 
#         USAGE: ./clang-format-server.sh 
# 
#   DESCRIPTION: 
# 
#        AUTHOR: Peng Wang
#       CREATED: 11/19/15 16:52
#===============================================================================

set -o nounset                              # Treat unset variables as an error
PY_PATH="clang-format-server.py"
LOG_PATH="/tmp/clang-format-server.log"
PID_PATH="/tmp/clang-format-server.pid"
twistd -l ${LOG_PATH} --pidfile ${PID_PATH} -y ${PY_PATH}
