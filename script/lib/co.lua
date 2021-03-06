

local _M = {}

local CoCreate = coroutine.create
local CoRunning = coroutine.running
local CoYield = coroutine.yield
local CoResume = coroutine.resume

local coPool_ = {}
local coWait_ = {}

local CO_STATE = {
	eEXIT = 1,
	eWAIT = 2	
}

local kMAX_SESSION = math.maxinteger
local session_ = 1
local mainCo_ = CoRunning()

local function CoCreate(func)
	local co = table.remove(coPool_)
	if co == nil then
		co = coroutine.create(function(...)
			func(...)
			while true do
				func = nil
				coPool_[#coPool_+1] = co
				func = CoYield(CO_STATE.eEXIT)
				func(CoYield())
			end
		end)
	else
		CoResume(co, func)
	end
	return co
end

local function CoMonitor(co,ok,state,session)
	if ok then
		if state == CO_STATE.eWAIT then
			coWait_[session] = co
		else
			assert(state == CO_STATE.eEXIT)
		end
	else
		RUNTIME_LOG:ERROR(debug.traceback(co,tostring(state)))
	end
end

function _M.Fork(func,...)
	local co = CoCreate(func)
	CoMonitor(co,CoResume(co,...))
end

function _M.Wakeup(session,...)
	local co = coWait_[session]
	coWait_[session] = nil
	if co then
		CoMonitor(co,CoResume(co,...))
	else
		RUNTIME_LOG:ERROR(string.format("error wakeup:session:%s not found",session))
	end
end

function _M.Wait(session)
	local co = CoRunning()
	if co == mainCo_ then
		error("cannot wait in main co,wait op should run in fork")
	end
	return CoYield(CO_STATE.eWAIT,session)
end

function _M.Sleep(time)
	local session = _M.GenSession()
	fish.StartTimer(time, 0, function ()
		_M.Wakeup(session)
	end)
	_M.Wait(session)
end

function _M.GenSession()
	if session_ >= kMAX_SESSION then
		session_ = 1
	end
	local session = session_
	session_ = session_ + 1
	return session
end

return _M
