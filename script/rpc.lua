local co = require "lib.co"
local socket = require "lib.socket"
local import = require "lib.import"

local encode = fish.Pack
local decode = fish.UnPack

rpcId_ = rpcId_ or nil
rpcName_ = rpcName_ or nil

channelCtx_ = channelCtx_ or {}

agentMgrChannel_ = agentMgrChannel_ or nil
sceneMgrChannel_ = sceneMgrChannel_ or nil

loginChannelCtx_ = loginChannelCtx_ or {}
agentChannelCtx_ = agentChannelCtx_ or {}
sceneChannelCtx_ = sceneChannelCtx_ or {}

local function AddChannel(id, name, channel)
	if name == "agentMgr" then
		agentMgrChannel_ = channel
	elseif name == "sceneMgr" then
		sceneMgrChannel_ = channel
	elseif name == "login" then
		loginChannelCtx_[id] = channel
	elseif name == "agent" then
		agentChannelCtx_[id] = channel
	elseif name == "scene" then
		sceneChannelCtx_[id] = channel
	end

	channelCtx_[channel] = {id = id, name = name, sessionCtx = {}}
end

local function RemoveChannel(channel)
	local ctx = channelCtx_[channel]
	channelCtx_[channel] = nil
	if channel.name == "agentMgr" then
		agentMgrChannel_ = nil
	elseif channel.name == "sceneMgr" then
		sceneMgrChannel_ = nil
	elseif channel.name == "login" then
		loginChannelCtx_[channel.id] = nil
	elseif channel.name == "agent" then
		agentChannelCtx_[channel.id] = nil
	elseif channel.name == "scene" then
		sceneChannelCtx_[channel.id] = nil
	end

	local sessionList = {}
	for session,callback in pairs(ctx.sessionCtx) do
		if callback == false then
			table.insert(sessionList,session)
		end
	end

	for _,session in ipairs(sessionList) do
		co.Wakeup(session, false, "bad channel")
	end
end


local function SendChannel(channel, method, args, callback)
	local session
	if callback then
		session = co.GenSession()
		local ctx = channelCtx_[channel]
		if ctx then
			ctx.sessionCtx[session] = callback
		end
	end

	local ptr, size = encode({method = method,session = session,args = args})
	channel:Write(2, ptr, size)
end

local function CallChannel(channel, method, args, timeout)
	local session = co.GenSession()

	local ctx = channelCtx_[channel]
	if ctx then
		ctx.sessionCtx[session] = false
	end

	local ptr, size = encode({method = method,session = session,args = args})
	channel:Write(2, ptr, size)
	
	if timeout then
		fish.StartTimer(timeout, 0, function ()
			co.Wakeup(session, {false, "timeout"})
		end)
	end

	local result = co.Wait(session)
	if not result[1] then
		error(result[2])
	end
	return table.unpack(result, 2)
end

local function RetChannel(channel, session, args)
	local ptr,size = encode({ret = true,session = session,args = args})
	channel:Write(2, ptr,size)
end

local function RunInCo(channel, session, moduleInst, funcInst, args)
	local result
	if moduleInst then
		result = {xpcall(funcInst, debug.traceback, moduleInst, table.unpack(args), channel)}
	else
		result = {xpcall(funcInst, debug.traceback, table.unpack(args), channel)}
	end

	if session then
		RetChannel(channel, session, table.unpack(result))
	end
end

function OnData(self, channel, data, size)
	local message = decode(data, size)
	if message.ret then
		local callback
		local ctx = channelCtx_[channel]
		if ctx then
			callback = ctx.sessionCtx[message.session]
			ctx.sessionCtx[message.session] = nil
		end

		if callback then
			if message.args[1] then
				co.Fork(callback, table.unpack(message.args, 2))
			end
		else
			co.Wakeup(message.session, message.args)
		end
	else
		local moduleInst
		local funcInst
		local module, seq, func = string.match(message.method, "(.*)([:.])([%w_]+)$")
		if seq == ":" or seq == "." then
			local moduleCtx = import.GetModule(module)
			if not moduleCtx then
				RUNTIME_LOG:ERROR_FM("server no such module:%s",module)
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
			RUNTIME_LOG:ERROR_FM("server no such method:%s", func)
			return
		end

		co.Fork(RunInCo, channel, message.session, moduleInst, funcInst, message.args)
	end
end

function OnClose(self, channel)
	local ctx = channelCtx_[channel]
	RemoveChannel(channel)
	RUNTIME_LOG:ERROR_FM("channel:%d,%s close", ctx.id, ctx.name)
end

function OnError(self, channel)
	local ctx = channelCtx_[channel]
	RemoveChannel(channel)
	RUNTIME_LOG:ERROR_FM("channel:%d,%s error", ctx.id, ctx.name)
end

function OnAccept(self, fd, addr)
	RUNTIME_LOG:ERROR_FM("accept server:%s", addr)
	socket.Bind(fd, 2, self, "OnData", "OnClose", "OnError")
end

function Listen(self, addr, id, name)
	assert(socket.Listen(addr, self, "OnAccept"))
	rpcId_ = id
	rpcName_ = name
	return true
end

function Connect(self, addr, timeout)
	local fd, reason = socket.Connect(addr)
	if not fd then
		return false, reason
	end
	local channel = socket.Bind(fd, 2, self, "OnData", "OnClose", "OnError")
	local result = CallChannel(channel, "rpc:Register", {env.appUid, env.appName, env.appType}, timeout)
	AddChannel(result.id, result.name, channel)
	return channel
end

function Register(self, appId, appName, appType, channel)
	RUNTIME_LOG:ERROR_FM("[%s]:[%s]", env.appName, appName)
	AddChannel(appId, appType, channel)
	return {env.appUid, env.appName, env.appType}
end

function SendAgentMgr(self, method, args, callback)
	if not agentMgrChannel_ then
		error("not connect agent mgr")
	end
	SendChannel(agentMgrChannel_, method, args, callback)
end

function CallAgentMgr(self, method, args)
	if not agentMgrChannel_ then
		error("not connect agent mgr")
	end
	return CallChannel(agentMgrChannel_, method, args)
end

function SendSceneMgr(self, method, args, callback)
	if not sceneMgrChannel_ then
		error("not connect scene mgr")
	end
	SendChannel(sceneMgrChannel_, method, args, callback)
end

function CallSceneMgr(self, method, args)
	if not sceneMgrChannel_ then
		error("not connect scene mgr")
	end
	return CallChannel(sceneMgrChannel_, method, args)
end

function SendLogin(self, id, method, args, callback)
	SendChannel(loginChannel_, method, args, callback)
end

function CallLogin(self, id, method, args)
	return CallChannel(loginChannel_, method, args)
end

function SendAgent(self, method, args, callback)
	SendChannel(worldChannel_, method, args, callback)
end

function CallAgent(self, method, args)
	return CallChannel(worldChannel_, method, args)
end

function SendScene(self, method, args, callback)
	SendChannel(worldChannel_, method, args, callback)
end

function CallScene(self, method, args)
	return CallChannel(worldChannel_, method, args)
end
