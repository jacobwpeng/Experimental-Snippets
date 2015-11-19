#!/usr/bin/env python

# Copyright (c) Twisted Matrix Laboratories.
# See LICENSE for details.

import sys
import struct
import pickle
import tempfile
import subprocess
import logging
from twisted.internet.protocol import Protocol, Factory
from twisted.internet import reactor
from twisted.python import log
from twisted.application.internet import TCPServer
from twisted.application.service import Application

TEMP_FORMAT_FILE_PATH='/tmp/.clang-format'

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

    with open(TEMP_FORMAT_FILE_PATH, 'w') as fd:
      fd.write(fmt['format_file'])

    proc = subprocess.Popen(['clang-format', '-style=file',
      '-assume-filename={}'.format(TEMP_FORMAT_FILE_PATH)],
        stdin = subprocess.PIPE,
        stdout = subprocess.PIPE)
    formatted = proc.communicate(fmt['file'])[0]

    #log.msg('Reply size: {}'.format(len(formatted)))
    self.transport.write(struct.pack('I', len(formatted)))
    self.transport.write(formatted)

logging.basicConfig(level=logging.WARNING)
f = Factory()
f.protocol = Echo

application = Application('ClangFormatServer')
service = TCPServer(56789, f)
service.setServiceParent(application)
