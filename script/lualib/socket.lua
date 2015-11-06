local fish = require "lualib.fish"
local SocketCore = require "SocketCore"
local Acceptor = require "Acceptor"
local Connector = require "Connector"

local _bufferSize = 1024*1024
local _M = {}

local _socket = {}

local _ev = {}

function _ev.OnAccept(source,fd)
	local socket = _socket[source]
	_socket[fd] = {type = "socket",co = nil,total = 0,need = 0,connected = true}
	socket.func(fd)
end

function _ev.OnConnect(source,done)
	local s = _socket[source]
	s.connected = false
	if done == 1 then
		s.connected = true
	end
	fish.Wakeup(s.co)
end

function _ev.OnClose(fd)
	local s = _socket[fd]
	s.connected = false
	if s.co ~= nil then
		fish.Wakeup(s.co)
	else
		_socket[fd] = nil
		SocketCore.Delete(s.core)
	end
end

function _ev.OnData(fd,data,size)
	local s = _socket[fd]
	
	s.total = SocketCore.SaveBuffer(s.core,data,size)
	if s.total >= s.need then
		fish.Wakeup(s.co)
	end
end

local _evFunc = {
	[1] = _ev.OnData,
	[2] = _ev.OnAccept,
	[3] = _ev.OnConnect,
	[4] = _ev.OnClose,
}

local function dispatch(source,_,ev,...)
	_evFunc[ev](source,...)
end

function _M.Listen(host,port,func)
	local acceptor = Acceptor.New()
	local fd = Acceptor.Listen(acceptor,host,port)
	assert(fd ~= -1,"listen error")
	_socket[fd] = {core = acceptor,type = "acceptor",func = func}
	return fd
end

function _M.Connect(host,port)
	local connector = Connector.New()
	local done,connected,fd = Connector.Connect(connector,host,port)

	if not done then
		Connector.Delete(connector)
		return false,connected
	end

	if connected then
		_socket[fd] = {type = "socket",co = nil,total = 0,need = 0,connected = true}
	else
		_socket[fd] = {type = "socket",co = coroutine.running(),total = 0,need = 0,connected = false}
		
		fish.Wait()
		
		Connector.Delete(connector)

		if _socket[fd].connected == false then
			_socket[fd] = nil
			return false,"host not reach"
		end
		_socket[fd].co = nil
	end
	
	return fd
end

function _M.Start(fd,buffsize)
	local s,err = SocketCore.New(fd,buffsize or _bufferSize)
	if not s then
		_socket[fd] = nil
		return false,err
	end
	_socket[fd].core = s
	return true
end

function _M.Read(fd,size)
	local s = _socket[fd]

	if size == nil then
		--read all
		if s.total > 0 then
			local total = s.total
			s.total = 0
			return SocketCore.ReadBuffer(s.core,total)
		else
			assert(not s.co)
			s.co = coroutine.running()
			s.need = 0
			fish.Wait()
			s.co = nil
		end

		if not s.connected then
			_socket[fd] = nil
			SocketCore.Delete(s.core)
			return false
		end
		local total = s.total
		s.total = 0
		return SocketCore.ReadBuffer(s.core,total)

	else
		if s.total >= size then
			s.total = s.total - size
			return SocketCore.ReadBuffer(s.core,size)
		else
			assert(not s.co)
			s.co = coroutine.running()
			s.need = size
			fish.Wait()
			s.co = nil
		end
		if not s.connected then
			_socket[fd] = nil
			SocketCore.Delete(s.core)
			return false
		end
		s.total = s.total - size
		return SocketCore.ReadBuffer(s.core,size)
	end
end

function _M.Send(fd,...)
	local s = _socket[fd]
	assert(s ~= nil,fd)
	assert(s.connected == true)
	return SocketCore.Send(s.core,...)
end

function _M.Close(fd)
	local s = _socket[fd]
	assert(s ~= nil,fd)
	if s.type == "socket" then
		assert(s.connected == true)
		local r = SocketCore.Close(s.core)
		if not r then
			s.co = coroutine.running()
			fish.Wait()
		end
		_socket[fd] = nil
		SocketCore.Delete(s.core)
	elseif s.type == "acceptor" then
		_socket[fd] = nil
		Acceptor.Close(s.core)
		Acceptor.Delete(s.core)
	else
		error(string.format("close error:%s",s.type))
	end
end


fish.Dispatcher("Socket",dispatch)

return _M