#!/usr/bin/env python

import os
import sys
import struct
import pickle
import socket

def ParseArgs():
  import argparse
  parser = argparse.ArgumentParser(description = 'Format some code file')
  parser.add_argument('-H', '--host', dest = 'host', action = 'store',
      default = '127.0.0.1', help = 'Host ip')
  parser.add_argument('-p', '--port', dest = 'port', action = 'store',
      default = 56789, type = int, help = 'Host port')
  parser.add_argument('-i', '--inplace', dest = 'inplace',
      action = 'store_true', help = 'Inplace edit file')
  parser.add_argument('-f', '--format-file', dest = 'format_file_path',
      action = 'store',
      default = os.path.join(os.path.expanduser('~'), '.clang-format'),
      help = 'Format configure path')
  parser.add_argument(dest = 'filenames', metavar = 'filename', nargs = '*')
  return parser.parse_args()

def Format(filename, args):
  with open(filename) as fd:
    file_content = fd.read()

  fmt = {
      'file': file_content,
      'format_file': args.format_file_content
  }

  serialized = pickle.dumps(fmt)
  l = len(serialized)

  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  sock.settimeout(3)

  try:
    addr = (args.host, args.port)
    sock.connect(addr)
    bytes = sock.send(struct.pack('I', l))
    data = serialized
    while len(data) != 0:
      bytes = sock.send(data)
      data = data[bytes:]

    reply_size_binary = sock.recv(4)
    reply_size = struct.unpack('I', reply_size_binary)[0]

    reply = ''
    while reply_size != 0:
      data = sock.recv(reply_size)
      reply_size -= len(data)
      reply += data

    if args.inplace:
      with open(filename, 'w') as fd:
        fd.write(reply)
    else:
      sys.stdout.write(reply)
  except socket.timeout, e:
    print 'Communicate to server timeout'
    sys.exit(-1)

def main(args):
  if not os.path.isfile(args.format_file_path):
    print 'No {} file exists or it\'s not a file.'.format(args.format_file_path)
    sys.exit(-1)

  with open(args.format_file_path) as fd:
    args.format_file_content = fd.read()

  for filename in args.filenames:
    Format(filename, args);

  sys.exit(0)

if __name__ == '__main__':
  main(ParseArgs())
