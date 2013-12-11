#!/usr/bin/env lua

local aio = require 'aio'

function AddTask(input, out, piece_size)
    assert( input )
    assert( out )
    local r_handle = aio.new(input, 'r')
    local w_handle = aio.new(out, 'w')

    piece_size = piece_size and piece_size or 100
    return coroutine.create( function()
                                 local offset = 0
                                 while 1 do
                                     local data = r_handle:read(offset, piece_size)
                                     if not data then break end
                                     local bytes_write = w_handle:write(offset, data)
                                     io.write( string.format('bytes write : %d\n', bytes_write) )
                                     offset = offset + string.len(data)
                                 end
                             end )
end
local idx = 0
local co_t = {}
local co = AddTask('/tmp/Makefile', '/tmp/dump', 1024)
co_t[#co_t+1] = co

co = AddTask('/tmp/CMakeCache.txt', '/tmp/CMakeCache.txt.dump', 1024)
co_t[#co_t+1] = co
coroutine.resume( co_t[1] )
coroutine.resume( co_t[2] )
while 1 do
    local flag = true
    for _, v in ipairs(co_t) do
        flag = flag and coroutine.status(v) == 'dead'
    end
    if flag then break end
end
