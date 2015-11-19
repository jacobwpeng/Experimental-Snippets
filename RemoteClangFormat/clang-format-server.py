#!/usr/bin/env python

# Copyright (c) Twisted Matrix Laboratories.
# See LICENSE for details.

import sys
import struct
import pickle
import tempfile
import subprocess
from twisted.internet.protocol import Protocol, Factory
from twisted.internet import reactor
from twisted.python import log

### Protocol Implementation

# This is just about the simplest possible protocol
class Echo(Protocol):
  def __init__(self):
    self._len = None
    self._data = ''

  def dataReceived(self, data):
    """
    As soon as any data is received, write it back.
    """
    #log.msg('Receive {} Bytes'.format(len(data)))
    self._data += data
    if len(self._data) < 4:
      return
    if self._len is None:
      self._len = struct.unpack('I', self._data[:4])[0]
      self._data = self._data[4:]
    #log.msg('Payload size: {}'.format(self._len))
    #log.msg('Total: {}'.format(len(self._data)))

    if self._len and len(self._data) == self._len:
      self.OnMessage(self._data)

  def OnMessage(self, data):
    fmt = pickle.loads(data)
    print fmt

    with open('.clang-format', 'w') as fd:
      fd.write(fmt['format_file'])

    proc = subprocess.Popen(['clang-format'],
        stdin = subprocess.PIPE,
        stdout = subprocess.PIPE)
    formatted = proc.communicate(fmt['file'])[0]

    #log.msg('Reply size: {}'.format(len(formatted)))
    self.transport.write(struct.pack('I', len(formatted)))
    self.transport.write(formatted)

def main():
  log.startLogging(sys.stdout)
  f = Factory()
  f.protocol = Echo
  reactor.listenTCP(56789, f)
  reactor.run()

if __name__ == '__main__':
  main()
