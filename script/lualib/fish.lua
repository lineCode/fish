local Core = require "Core"
local Timer = require "Timer"
local Server = require "Server"
local util = require "lualib.util"

local _M = {}

local _coCreate = coroutine.create
local _coResume = coroutine.resume
local _coYield = coroutine.yield

local _coPool = {}
local _coInfo = {}
local _coSession = {}
local _sessionCo = {}
local _waitCo = {}
local _wakeupCo = {}

local _forkCo = {}

local _mainCo = coroutine.running()

local _serverInfo = {}

--fixme:delete when gc
local _timerPool = setmetatable({},{__gc = function (tbl)
		for _,timer in pairs(tbl) do
			Timer.Delete(timer)
		end
	end})
local _timerMgr = setmetatable({},{__gc = function (tbl)
		for _,timer in pairs(tbl) do
			Timer.Delete(timer)
		end
	end})

local _timerId = 1

local _dispatchFunc = {}

local _DISPATCH_TYPE = 
{
	Client = 0,
	Server = 1,
	Socket = 2,
	Mongo = 3,
	Timer = 4
}

local _coState = {
	Call = 1,
	Sleep = 2,
	Wait = 3,
	Quit = 4,
}

local function coCreate(func)
	local co = table.remove(_coPool)
	if co == nil then
		co = coroutine.create(function(...)
			func(...)
			while true do
				func = nil
				_coPool[#_coPool+1] = co
				func = _coYield(_coState.Quit)
				func(_coYield())
			end
		end)
	else
		coroutine.resume(co, func)
	end
	return co
end

local function coMonitor(co,result,cmd,packType,data)
	if result then
		if cmd == _coState.Call then
			local session = _coSession[co]
			_coSession[co] = nil
			_sessionCo[session] = co
		elseif cmd == _coState.Sleep then
			local session = _coSession[co]
			_coSession[co] = nil
			_sessionCo[session] = co
		elseif cmd == _coState.Wait then
			_waitCo[co] = true
		elseif cmd == _coState.Quit then
			if _coInfo[co] ~= nil then
				_coInfo[co] = nil
			end
		else
			assert(false,string.format("error cmd:%d",cmd))
		end
	else
		local error = {cmd,debug.traceback(co)}
		Core.Log(table.concat(error,"\n"))
	end
end

local function runFork()
	while true do
		local key,co = next(_forkCo)
		if co == nil then
			break
		end
		_forkCo[key] = nil
		coMonitor(co,_coResume(co))
	end
end

local function runWakeup()
	while true do
		local key,co = next(_wakeupCo)
		if co == nil then
			break
		end
		_wakeupCo[key] = nil
		coMonitor(co,_coResume(co))
	end
end

local function getTimer()
	local key,timer = next(_timerPool)
	if timer == nil then
		timer = Timer.New()
	else
		_timerPool[key] = nil
	end
	return timer
end

function _M.Timeout(ti,func)
	local timer = getTimer()

	local session = Core.GenSession()
	
	Timer.Start(timer,ti,session)
	
	local id = _timerId
	_timerId = _timerId + 1
	_timerMgr[id] = {timer = timer,cancel = false}

	local co = coCreate(function ()
		local timerInfo = _timerMgr[id]
		if timerInfo.cancel == false then
			local r,err = xpcall(func,function (err)
				local errlist = {}
				table.insert(errlist,err)
				table.insert(errlist,debug.traceback())
				return table.concat(errlist,"\n")
			end)
			if not r then
				_M.Log(err)
			end
		end
		_timerMgr[id] = nil
		table.insert(_timerPool,timer)
	end)

	_sessionCo[session] = co 
	return id
end

function _M.TimeoutCancel(id)
	local timerInfo = _timerMgr[id]
	assert(timerInfo ~= nil and timerInfo.cancel == false)
	timerInfo.cancel = true
end

function _M.Sleep(ti)
	local timer = getTimer()
	local session = Core.GenSession()
	_coSession[coroutine.running()] = session
	Timer.Start(timer,ti,session)
	_coYield(_coState.Sleep)
	table.insert(_timerPool,timer)
end

function _M.Wait()
	_coYield(_coState.Wait)
end

function _M.Wakeup(co)
	assert(_waitCo[co] ~= nil)
	_waitCo[co] = nil
	table.insert(_wakeupCo,co)
end

function _M.Call(addr,method,...)
	local session = Core.GenSession()
	
	Core.Send(addr,false,session,method,...)
	
	_coSession[coroutine.running()] = session
	local _,_,data = _coYield(_coState.Call)
	return data
end

function _M.Ret(...)
	local co = coroutine.running()
	local info = _coInfo[co]
	Core.Send(info.source,true,info.session,0,...)
	return
end

function _M.Send(addr,method,...)
	Core.Send(addr,false,0,method,...)
end

function _M.SendClient(addr,method,data)
	Core.SendClient(addr,method,data)
end

function _M.Fork(func,...)
	local args = {...}
	local co = coCreate(function ()
		func(table.unpack(args))
	end)
	table.insert(_forkCo,co)
end

function _M.Dispatcher(proto,func)
	local type = _DISPATCH_TYPE[proto]
	assert(type,string.format("error dispatcher type:%s",proto))
	_dispatchFunc[type] = func
end

local function dispatchMessage(proto,source,response,session,...)
	if response == false then
		local func = _dispatchFunc[proto]
		assert(func ~= nil,proto)
		local co = coCreate(func)
		_coInfo[co] = {proto = proto,source = source,session = session}
		coMonitor(co,_coResume(co,source,session,...))
	else
		local co = _sessionCo[session]
		if co then
			coMonitor(co,_coResume(co,...))
			_sessionCo[session] = nil
		else
			assert(false)
		end
	end
	runWakeup()
	runFork()
end

function _M.Start(func)
	_M.Fork(func)
	runFork()
end

function _M.Log(str)
	Core.Log(str)
end

function _M.Now()
	return Core.Now()
end

function _M.Timestamp()
	return Core.Timestamp()
end

function _M.Timestamp2Second(ti)
	return Core.TimestampToSecond(ti)
end

function _M.Stop()
	return Core.Stop()
end

local function dispatchServer(source,session,type,param,...)
	local server = _serverInfo[source]
	if type == 1 then
		server.onData(source,param,...)
	else
		if param == true then
			server.onStart(source)
		else
			server.onClose(source)
		end
	end
end

function _M.StartServer(host,port,onData,onStart,onClose)
	local socket = require "lualib.socket"
	local fd = socket.Listen(host,port,function (rfd)
		_serverInfo[rfd] = {type = "server",
							onData = onData,
							onStart = onStart,
							onClose = onClose}

		local server,err = Server.New(rfd,1024)
		if not server then
			return false,err
		end
		_serverInfo[rfd].core = server
	end)
	_serverInfo[fd] = {type = "acceptor"}

	return fd
end

function _M.ConnectServer(host,port,onData,onStart,onClose)
	local socket = require "lualib.socket"
	local fd = assert(socket.Connect(host,port))
	_serverInfo[fd] = {type = "server",
					   onData = onData,
					   onStart = onStart,
					   onClose = onClose}
	local server,err = Server.New(fd,1024)
	if not server then
		return false,err
	end
	_serverInfo[fd].core = server
	return fd
end

function _M.Close(source)
	if _serverInfo[source].type == "acceptor" then
		socket.Close(source)
	else
		local serverCore = _serverInfo[source].core
		Server.Close(serverCore)
		Server.Delete(serverCore)
	end
	_serverInfo[source] = nil
end

function _M.Clear()
	_coPool = {}
end

_M.Dispatcher("Server",dispatchServer)

Core.CallBack(dispatchMessage)

return _M