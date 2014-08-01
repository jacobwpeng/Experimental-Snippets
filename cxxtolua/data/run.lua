#!/usr/bin/env lua

--print 'Before require'
--local array = require 'array'
local faint = require 'faint'

--faint.google = 2
for i = 1, 5 do
    faint.val = i
    print (faint.val)
end

--print (faint.std_string.size())
--print (faint.std_vector.size())

--print (faint.std_string.fool)
--print 'After require'

--local arr = array.New()
----local arr = array.DynamicArray()
--for i = 1, 100 do
--    array.Append(arr, i)
--end
--
--print (array.Get(arr, 98))
--print (array.Size(arr))
--
----array.TestOverload(arr)
--array.TestOverload(arr, 'str')
--array.TestOverload(arr, -123456)
----array.TestOverload(arr, 1, 'str')
--
--local msg = array.GetMessage()
--array.PrintMessage(msg)
--print (msg)

--array.Delete(arr)
