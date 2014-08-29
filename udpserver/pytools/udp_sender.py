#!/usr/bin/env python

from __future__ import division
import math
import socket
import random
import fileinput
import arena_message_pb2

ip = '127.0.0.1'
port = 6789

random.seed()
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.settimeout(1.0)
points = {}
req = arena_message_pb2.Request()
response = arena_message_pb2.Response()
timeout = 0

def UpdateSelf(uin, point):
    req.type = arena_message_pb2.Request.UPDATE_SELF
    req.self = uin
    req.points = point
    try:
        sock.sendto(req.SerializeToString(), (ip, port))
        data, addr = sock.recvfrom(65536)
    except socket.timeout as e:
        global timeout
        timeout += 1
        return

    response.ParseFromString(data)
    assert (response.status == arena_message_pb2.Response.OK)

def FightOpponent(uin, point):
    req.type = arena_message_pb2.Request.FIND_OPPONENT
    req.self = uin
    req.points = point
    try:
        sock.sendto(req.SerializeToString(), (ip, port))
        data, addr = sock.recvfrom(65536)
    except socket.timeout as e:
        global timeout
        timeout += 1
        return

    respjnse.ParseFromString(data)
    if response.status == arena_message_pb2.Response.NO_OPPONENT:
        UpdateSelf(uin, point)
    else:
        opponent = response.opponent
        assert (opponent != uin)

        assert opponent in points, opponent
        delta = points[opponent] - point
        win = bool(random.getrandbits(1))
        w = win and 1 or 0
        delta_point = math.floor(20 * (w - (1 / 1 + pow(10, (delta/400)))))
        new_point = int(point + delta_point)
        if new_point < 0: new_point = 0
        UpdateSelf(uin, new_point)
        points[uin] = new_point

# 20w
while 1:
    if len(points) >= 200000: break
    uin = random.randrange(1000, 2**32)
    point = 200
    points[uin] = 200

for i in xrange(1, 2**32):
    uin = random.choice(points.keys())
    FightOpponent(uin, points[uin])
    if i % 100 == 0: print i
    if timeout != 0 and timeout % 5 == 0: print 'timeout[%d]' % timeout
