local socket = require "lib.socket"
local co = require "lib.co"
local import = require "lib.import"

local encode = fish.Pack
local decode = fish.UnPack

SessionCtx_ = SessionCtx_ or {}

rpcId_ = rpcId_ or nil
rpcName_ = rpcName_ or nil
channelCtx_ = channelCtx_ or {}

loginChannel_ = loginChannel_ or nil
worldChannel_ = worldChannel_ or nil

local function SetChannel(name, channel)
	if name == "login" then
		loginChannel_ = channel
	elseif name == "world" then
		worldChannel_ = channel
	end
end

local function SendChannel(channel, method, args, callback)
	local session = 0
	if callback then
		session = co.GenSession()
		SessionCtx_[session] = callback 
	end

	local ptr, size = encode({method = method,session = session,args = args})
	channel:Write(2, ptr, size)
end

local function CallChannel(channel, method, args, timeout)
	local session = co.GenSession()

	local ptr, size = encode({method = method,session = session,args = args})
	channel:Write(2, ptr, size)
	
	if timeout then
		fish.StartTimer(timeout, 0, function ()
			co.Wakeup(session, false, "timeout")
		end)
	end

	local ok,value = co.Wait(session)
	if not ok then
		error(value)
	end
	return value
end

local function RetChannel(channel, session, ...)
	local ptr,size = encode({ret = true,session = session,args = {...}})
	channel:Write(2, ptr,size)
end

local function RunInCo(channel, session, moduleInst, funcInst, args)
	local ok
	local result
	if moduleInst then
		ok, result = xpcall(funcInst, debug.traceback, moduleInst, args, channel)
	else
		ok, result = xpcall(funcInst, debug.traceback, args, channel)
	end

	if session then
		if ok then
			RetChannel(channel, session, true, result)
		else
			RetChannel(channel, session, false, result)
		end
	end
end

function OnData(self, channel, data, size)
	local message = decode(data, size)
	if message.ret then
		local callback = SessionCtx_[message.session]
		if callback then
			if message.args[1] then
				co.Fork(callback, table.unpack(message.args, 2))
			end
			SessionCtx_[message.session] = nil
		else
			co.Wakeup(message.session, table.unpack(message.args))
		end
	else
		local moduleInst
		local funcInst
		local module, seq, func = string.match(message.method, "(.*)([:.])([%w_]+)$")
		if seq == ":" or seq == "." then
			local moduleCtx = import.GetModule(module)
			if not moduleCtx then
				fish.Log("runtime", string.format("server no such module:%s",module))
				return
			end

			funcInst = moduleCtx[func]
			if seq == ":" then
				moduleInst = moduleCtx
			end
		else
			funcInst = _G[func]
		end

		if not funcInst then
			fish.Log("runtime", string.format("server no such method:%s", func))
			return
		end

		co.Fork(RunInCo, channel, message.session, moduleInst, funcInst, message.args)
	end
end

function OnClose(self, channel)
	fish.Log("rpc", "server close")
end

function OnError(self, channel)
	fish.Log("rpc", "server error")
end

function OnAcceptServer(self, fd, addr)
	fish.Log("rpc", string.format("accept server:%s", addr))
	socket.Bind(fd, 2, self, "OnData", "OnClose", "OnError")
end

function Listen(self, addr, id, name)
	assert(socket.Listen(addr, self, "OnAcceptServer"))
	rpcId_ = id
	rpcName_ = name
	return true
end

function Connect(self, addr, id, name, timeout)
	local fd, reason = socket.Connect(addr)
	if not fd then
		return false, reason
	end
	local channel = socket.Bind(fd, 2, self, "OnData", "OnClose", "OnError")
	local result = CallChannel(channel, "rpc:Register", {id = id, name = name}, timeout)
	SetChannel(result.name, channel)
	channelCtx_[result.id] = channel

	return channel
end

function Register(self, args, channel)
	fish.Log("rpc", string.format("server:%d,%s register from %d,%s", args.id, args.name, rpcId_, rpcName_))

	channelCtx_[args.id] = channel
	SetChannel(args.name, channel)
	return {id = rpcId_, name = rpcName_}
end

function SendAgent(self, id, method, args, callback)
	local channel = channelCtx_[id]
	SendChannel(channel, method, args, callback)
end

function CallAgent(self, id, method, args)
	local channel = channelCtx_[id]
	return CallChannel(channel, method, args)
end

function SendScene(self, id, method, args, callback)
	local channel = channelCtx_[id]
	SendChannel(channel, method, args, callback)
end

function CallScene(self, id, method, args)
	local channel = channelCtx_[id]
	return CallChannel(channel, method, args)
end

function SendLogin(self, method, args, callback)
	SendChannel(loginChannel_, method, args, callback)
end

function CallLogin(self, method, args)
	return CallChannel(loginChannel_, method, args)
end

function SendWorld(self, method, args, callback)
	SendChannel(worldChannel_, method, args, callback)
end

function CallWorld(self, method, args)
	return CallChannel(worldChannel_, method, args)
end
