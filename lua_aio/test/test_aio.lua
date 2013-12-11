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
                                     w_handle:write(offset, data)
                                     offset = offset + string.len(data)
                                 end
                             end )
end

local co = AddTask('/tmp/Makefile', '/tmp/dump', 1024)
coroutine.resume(co)
while coroutine.status(co) ~= 'dead' do end
