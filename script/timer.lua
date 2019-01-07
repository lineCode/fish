local co = require "co"

local _M = {}

local function CreateTimer(interval,freq,inst,method,...)
	local args = {...}

	local __timer = rawget(inst,"__timer")
	if not __timer then
		__timer = {}
		rawset(inst,"__timer",__timer)
	end

	local timerId = fish.StartTimer(interval,freq,function ()
		if freq == 0 then
			__timer[timerId] = nil
		end
		co.Fork(inst[method],inst,table.unpack(args))
	end)
	__timer[timerId] = true 
	return timerId
end

function _M.Callout(interval,inst,method,...)
	return CreateTimer(interval,interval,inst,method,...)
end

function _M.CalloutAfter(interval,inst,method,...)
	return CreateTimer(interval,0,inst,method,...)
end

function _M.RemoveTimer(inst,timerId)
	local __timer = rawget(inst,"__timer")
	assert(__timer ~= nil)
	assert(__timer[timerId],string.format("no such timer:%s",timerId))
	fish.CancelTimer(timerId)
	__timer[timerId] = nil
end

function _M.RemoveAll(inst)
	local __timer = rawget(inst,"__timer")
	if not __timer then
		return
	end
	for timerId in pairs(__timer) do
		fish.CancelTimer(timerId)
	end
	rawset(inst,__timer,{})
end

return _M
