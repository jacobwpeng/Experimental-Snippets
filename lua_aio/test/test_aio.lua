#!/usr/bin/env lua

local aio = require 'aio'

local r_handle = aio.new('Makefile', 'r')
local w_handle = aio.new('/tmp/Makefile', 'w')

local promises = {} 
local offset = 0
local size = 1024

for i = 1, 5 do
    promises[i] = r_handle:read(offset, size)
    offset = offset + size
end

aio.wait( promises )

local write_promises = {}
offset = 0
for i = 1, 5 do
    local data = promises[i]:retrieve()
    local len = string.len(data)
    promises[i]:destroy()
    write_promises[i] = w_handle:write(offset, data)
    offset = offset + len
end

aio.wait( write_promises )
