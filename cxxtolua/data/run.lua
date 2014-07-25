#!/usr/bin/env lua

local array = require 'array'

--local arr = array.New()
local arr = array.DynamicArray()
for i = 1, 100 do
    array.Append(arr, i)
end

print (array.Get(arr, 98))
print (array.Size(arr))

array.TestOverload(arr)
array.TestOverload(arr, 'str')
array.TestOverload(arr, 1)
array.TestOverload(arr, 1, 'str')

local msg = array.GetMessage()
array.PrintMessage(msg)
print (msg)

array.Delete(arr)
