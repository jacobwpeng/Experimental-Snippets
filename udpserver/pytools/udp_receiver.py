#!/usr/bin/env python

import sys
import socket

ip = '127.0.0.1'
port = 6789
max_buffer_len = 1024

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.bind((ip, port))

while True:
    data, addr = sock.recvfrom(max_buffer_len)
    if len(data): print data, 
