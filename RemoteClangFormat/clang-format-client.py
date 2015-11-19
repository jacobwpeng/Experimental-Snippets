#!/usr/bin/env python

import os
import sys
import struct
import pickle
import socket

HOME = os.path.expanduser('~')
FORMAT_FILE_PATH = os.path.join(HOME, '.clang-format')
CLANG_FORMAT_SERVER_IP = '10.6.224.83'
CLANG_FORMAT_SERVER_PORT = 56789

if not os.path.isfile(FORMAT_FILE_PATH):
  print 'No {} file exists or it\'s not a file'.format(FORMAT_FILE_PATH)
  sys.exit(-1)

with open(sys.argv[1]) as fd:
  file_content = fd.read()

with open(FORMAT_FILE_PATH) as fd:
  format_file_content = fd.read()

fmt = {
    'file': file_content,
    'format_file': format_file_content
}

serialized = pickle.dumps(fmt)
l = len(serialized)
print l

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.settimeout(3)

try:
  addr = (CLANG_FORMAT_SERVER_IP, CLANG_FORMAT_SERVER_PORT)
  sock.connect(addr)
  bytes = sock.send(struct.pack('I', l))
  data = serialized
  while len(data) != 0:
    bytes = sock.send(data)
    data = data[bytes:]

  reply_size_binary = sock.recv(4)
  reply_size = struct.unpack('I', reply_size_binary)[0]
  print 'Reply size: {}'.format(reply_size)

  reply = ''
  while reply_size != 0:
    data = sock.recv(reply_size)
    reply_size -= len(data)
    reply += data

  with open(sys.argv[1], 'w') as fd:
    fd.write(reply)
except socket.timeout, e:
  print 'Communicate to server timeout'
  sys.exit(-1)

sys.exit(0)
