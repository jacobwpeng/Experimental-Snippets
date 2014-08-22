#!/usr/bin/env python

import socket
import fileinput

ip = '127.0.0.1'
port = 6789

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

for line in fileinput.input():
    sock.sendto(line, (ip, port))
