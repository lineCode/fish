local co = require "lib.co"
local socket = require "lib.socket"
local import = require "lib.import"

local encode = fish.Pack
local decode = fish.UnPack

channelCtx_ = channelCtx_ or {}

dbChannel_ = dbChannel_
agentMasterChannel_ = agentMasterChannel_
loginMasterChannel_ = loginMasterChannel_
sceneMasterChannel_ = sceneMasterChannel_

loginChannelCtx_ = loginChannelCtx_ or {}
agentChannelCtx_ = agentChannelCtx_ or {}
sceneChannelCtx_ = sceneChannelCtx_ or {}

listener_ = listener_

local function AddChannel(appId, appType, channel, reconnect)
	if appType == APP_TYPE.DB then
		dbChannel_ = channel
	elseif appType == APP_TYPE.AGENT then
		agentChannelCtx_[appId] = channel
	elseif appType == APP_TYPE.AGENT_MASTER then
		agentMasterChannel_ = channel
	elseif appType == APP_TYPE.LOGIN then
		loginChannelCtx_[appId] = channel
	elseif appType == APP_TYPE.LOGIN_MASTER then
		loginMasterChannel_ = channel
	elseif appType == APP_TYPE.SCENE then
		sceneChannelCtx_[appId] = channel
	elseif appType == APP_TYPE.SCENE_MASTER then
		sceneMasterChannel_ = channel
	end

	local session
	if reconnect then
		session = co.GenSession()
	end
	local ctx = channelCtx_[channel]
	ctx.appId = appId
	ctx.appType = appType
	ctx.session = session
	return session
end

local function RemoveChannel(channel)
	local ctx = channelCtx_[channel]
	channelCtx_[channel] = nil

	if ctx.appType == APP_TYPE.DB then
		dbChannel_ = nil
	elseif ctx.appType == APP_TYPE.AGENT then
		agentChannelCtx_[ctx.appId] = nil
	elseif ctx.appType == APP_TYPE.AGENT_MASTER then
		agentMasterChannel_ = nil
	elseif ctx.appType == APP_TYPE.LOGIN then
		loginChannelCtx_[ctx.appId] = nil
	elseif ctx.appType == APP_TYPE.LOGIN_MASTER then
		loginMasterChannel_ = nil
	elseif ctx.appType == APP_TYPE.SCENE then
		sceneChannelCtx_[ctx.appId] = nil
	elseif ctx.appType == APP_TYPE.SCENE_MASTER then
		sceneMasterChannel_ = nil
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

	if ctx.session then
		co.Wakeup(ctx.session)
	end
end


local function SendChannel(channel, method, args, callback)
	local session
	if callback then
		session = co.GenSession()
		local ctx = channelCtx_[channel]
		ctx.sessionCtx[session] = callback
	end

	local ptr, size = encode({method = method,session = session,args = args})
	channel:Write(2, ptr, size)
end

local function CallChannel(channel, method, args, timeout)
	local session = co.GenSession()

	local ctx = channelCtx_[channel]
	ctx.sessionCtx[session] = false

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
		local ctx = channelCtx_[channel]
		local callback = ctx.sessionCtx[message.session]
		ctx.sessionCtx[message.session] = nil

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
	local channel = socket.Bind(fd, 2, self, "OnData", "OnClose", "OnError")
	channelCtx_[channel] = {sessionCtx = {}}
end

function Listen(self, addr)
	if self.listener_ then
		error("rpc already listen")
	end
	self.listener_ = socket.Listen(addr, self, "OnAccept")
	return self.listener_
end

local function DoConnect(self, addr, reconnect)
	local function ChannelInit(fd)
		local channel = socket.Bind(fd, 2, self, "OnData", "OnClose", "OnError")
		channelCtx_[channel] = {sessionCtx = {}}
		local appUid, appName, appType = CallChannel(channel, "rpc:Register", {env.appUid, env.appName, env.appType})
		local session = AddChannel(appUid, appName, appType, reconnect)
		return channel, session
	end

	if reconnect then
		while true do
			local fd, reason = socket.Connect(addr)
			if not fd then
				co.Sleep(1)
			else
				return ChannelInit(fd)
			end
		end
	end

	local fd, reason = socket.Connect(addr)
	if not fd then
		return false, reason
	end
	return ChannelInit(fd)
end

function Connect(self, addr, reconnect)
	if not reconnect then
		return DoConnect(self, addr)
	end

	local channel, session = DoConnect(self, addr, reconnect)
	if reconnect then
		co.Fork(function ()
			while true do
				co.Wait(session)
				channel, session = DoConnect(self, addr, reconnect)
			end
		end)
	end
	return channel
end

function Register(self, appId, appName, appType, channel)
	RUNTIME_LOG:ERROR_FM("[%s]:[%s]", env.appName, appName)
	AddChannel(appId, appType, channel)
	return {env.appUid, env.appName, env.appType}
end

function SendAgentMaster(self, method, args, callback)
	if not agentMasterChannel_ then
		error("not connect agent master")
	end
	SendChannel(agentMasterChannel_, method, args, callback)
end

function CallAgentMaster(self, method, args)
	if not agentMasterChannel_ then
		error("no agent master")
	end
	return CallChannel(agentMasterChannel_, method, args)
end

function SendSceneMaster(self, method, args, callback)
	if not sceneMasterChannel_ then
		error("no scene master")
	end
	SendChannel(sceneMasterChannel_, method, args, callback)
end

function CallSceneMaster(self, method, args)
	if not sceneMasterChannel_ then
		error("no scene master")
	end
	return CallChannel(sceneMasterChannel_, method, args)
end

function SendDb(self, id, method, args, callback)
	if not dbChannel_ then
		error("no db")
	end
	SendChannel(dbChannel_, method, args, callback)
end

function CallDb(self, id, method, args)
	if not dbChannel_ then
		error("no db")
	end
	return CallChannel(dbChannel_, method, args)
end

function SendLogin(self, id, method, args, callback)
	local channel = loginChannelCtx_[id]
	if not channel then
		error(string.format("no such login:%d channel", id))
	end
	SendChannel(channel, method, args, callback)
end

function CallLogin(self, id, method, args)
	local channel = loginChannelCtx_[id]
	if not channel then
		error(string.format("no such login:%d channel", id))
	end
	return CallChannel(channel, method, args)
end

function SendAgent(self, id, method, args, callback)
	local channel = agentChannelCtx_[id]
	if not channel then
		error(string.format("no such agent:%d channel", id))
	end
	SendChannel(channel, method, args, callback)
end

function CallAgent(self, id, method, args)
	local channel = agentChannelCtx_[id]
	if not channel then
		error(string.format("no such agent:%d channel", id))
	end
	return CallChannel(channel, method, args)
end

function SendScene(self, id, method, args, callback)
	local channel = sceneChannelCtx_[id]
	if not channel then
		error(string.format("no such scene:%d channel", id))
	end
	SendChannel(channel, method, args, callback)
end

function CallScene(self, id, method, args)
	local channel = sceneChannelCtx_[id]
	if not channel then
		error(string.format("no such scene:%d channel", id))
	end
	return CallChannel(channel, method, args)
end
