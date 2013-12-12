local aio = require 'aio'

local r_handle = aio.new('readfile', 'r')
local w_handle = aio.new('writefile', 'w')

local test_1 = coroutine.create( function()
                                    while 1 do
                                        local nbytes, data = r_handle:read(0, 1000)
                                        if nbytes == false then 
                                            --got error
                                            break 
                                        else
                                            print(data)
                                        end
                                    end
                                end
                                )
local test_2 = coroutine.create( function() 
                                    while 1 do
                                        local data = r_handle:read(0, 1000)
                                        if not data then break end
                                        w_handle:write(0, data)
                                    end
                                end
                                )
local test_3 = coroutine.create( function()
                                   local data = 'test for write'
                                   local times = 1024
                                   while 1 do
                                       if times == 0 then break end
                                       w_handle:write(0, data)
                                       times = times - 1
                                   end
                               end
                               )
aio.add_task( test_1 )
aio.add_task( test_2 )

--立即返回,内部启动线程进行循环
aio.run()

local unfinished_task = false
while 1 do
    lock(handles)
    if len(handles) == 0 then wait_for_signal() end
    for handle in handles do
        for info in infos do
            if info.ret == 0 then
                --normal wakeup
                if read then
                    pushint(info->L, info->bytes)
                    if info->bytes ~= 0 then
                        pushlstring(info->L, info->buf, info->bytes)
                    end
                else if write then
                    pushint(info->L, info->bytes)
                end
                coroutine.resume(info->L)
                cleanup(info)
            elseif info.ret == 1 then
                pushbool(info->L, false)
                pushliteral(info->L, "error message")
                cleanup(info)
                --error return
            elseif info.ret == -1 then
                unfinished_task = true
                --still waiting for signal
            end
        end
    end
    unlock(handles)
    if unfinished_task == false then signal_main_thread() end
end

aio.add_task( test_3 )

--等待当前所有异步操作完成,查看所有handle是否有不为NULL的info
aio.wait_all()

if len(handles) == 0 then 
    return 0
else
    return wait_unfinished_task_signal()
end
