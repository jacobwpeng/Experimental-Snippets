#!/usr/bin/env lua

local aio = require 'aio'

function read(handle, offset, size)
    local promise = handle:read(offset, size)
    coroutine.yield(promise)
    return promise:retrieve()
end

function write(handle, offset, data)
    local promise = handle:write(offset, data)
    coroutine.yield(promise)
end

function AddTask(src, dst)
    return coroutine.create( 
        function()
            local r_handle = aio.new(src, 'r')
            local w_handle = aio.new(dst, 'w')

            local len = r_handle:len()
            local piece_size = 1048576 
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

function printf(fmt, ...) io.write( string.format(fmt, ...) ) end

local cos = {}
for _, filename in ipairs(arg) do
    local s, e = filename:find('.*/')
    local src = filename
    local dst = 'todel/' .. filename:sub(e+1)
    print(src, dst)
    cos[#cos+1] = AddTask(src, dst)
end


local t = {}
for _, co in ipairs(cos) do
    local res, promise = coroutine.resume(co)
    if promise then
        t[co] = promise
    end
end

while next(t) ~= nil do
    local co, _ = aio.waitone(t)
    local res, promise = coroutine.resume(co)
    if promise then
        t[co] = promise
    else
        t[co] = nil
    end
end


