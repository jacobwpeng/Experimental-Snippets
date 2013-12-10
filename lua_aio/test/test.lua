#!/usr/bin/env lua

--local aio = require 'aio'
local array = require 'array'

local arr = array.new(10)
local co = coroutine.create( function() 
                                local val = arr:get(5)
                                if val == 0 then
                                    print( val )
                                else
                                    coroutine.yield(val)
                                end
                            end 
                        )

local status = coroutine.resume(co)
coroutine.resume(co)

while 1 do
    local status, data = coroutine.resume(co, 0, 100)
    if status == false then break end
    coroutine.resume( co, data )
end
