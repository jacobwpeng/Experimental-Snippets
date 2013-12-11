#!/usr/bin/env lua

local aio = require 'aio'
local handle = aio.new('/tmp/Makefile', 'r')
local co = coroutine.create( function() 
                            handle:read(0, 100000)
                            local status, data = coroutine.yield()
                            --print( status, data )
                        end
                        )
coroutine.resume(co)
