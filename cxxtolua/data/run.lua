#!/usr/bin/env lua

local array = require 'array'

local arr = array.New()
for i = 1, 100 do
    array.Append(arr, i)
end

print (array.Get(arr, 98))
print (array.Size(arr))
array.Delete(arr)

local msg = array.GetMessage()
print (msg)
