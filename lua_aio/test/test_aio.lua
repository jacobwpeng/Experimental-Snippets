#!/usr/bin/env lua

function printf(fmt, ...)
    io.write( string.format(fmt, ...) )
end

function read(handle, offset, size)
    local promise = r_handle:read(offset, piece_size)
    coroutine.yield(promise)
    return promise:retrieve()
end

function write(handle, offset, data)
    local promise = w_handle:write(offset, data)
    coroutine.yield(promise)
end

function AddTask(src, dst)
    return coroutine.create( 
        function()
            local r_handle = aio.new(src, 'r')
            local w_handle = aio.new(dst, 'w')

            local len = r_handle:len()
            local piece_size = 1024
            local part = math.ceil(len/piece_size)
            local offset = 0

            for i = 1, part do
                local data = read(r_handle, offset, piece_size)
                write(w_handle, offset, data)
                offset = offset + piece_size
            end
        end
    )
end

function print_table(t)
    for k,v in pairs(t) do
        io.write( string.format("%q -> ", k) )
        print(v)
    end
    print(string.rep('*', 80))
end

-- TODO : add wait_one and dispatcher

local aio = require 'aio'

local r_handle = aio.new('Makefile', 'r')
local w_handle = aio.new('/tmp/Makefile', 'w')

local promises = {} 
local offset = 0
local size = 1024
local len = r_handle:len()
local part = math.ceil( len/size )

printf("len = %d, part = %d\n", len, part)

for i = 1, part do
    promises[tostring(i)] = r_handle:read(offset, size)
    offset = offset + size
end

--do anything you want
--for i = 1, 100000000 do
--end
--print("done loop")

--if you want results, just wait
--if all operation in promises are done, then wait return immediately
--local k, v = aio.waitone(promises)
aio.wait(promises)

local write_promises = {}
offset = 0
for i = 1, part do
    local data = promises[tostring(i)]:retrieve()
    promises[tostring(i)]:destroy()
    local len = string.len(data)
    write_promises[i] = w_handle:write(offset, data)
    offset = offset + len
end

aio.wait(write_promises)
