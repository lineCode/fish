local co = require "co"

local acceptorCtx_ = {}
local channelCtx_ = setmetatable({},{__mode = "k"})

local _M = {}


local function OnData(channel, data, size)
	local ctx = channelCtx_[channel]
	ctx.inst[ctx.onData](ctx.inst, data, size)
end

local function OnClose(chanel)
	local ctx = channelCtx_[channel]
	ctx.inst[ctx.onClose](ctx.inst)
end

local function OnError(channel)
	local ctx = channelCtx_[channel]
	ctx.inst[ctx.onError](ctx.inst)
end


local meta = {}

function meta:Close()
	self.acceptor_:Close()
	acceptorCtx_[self.acceptor_] = nil
end

function _M.Listen(ip, port, inst, method)
	local acceptor = fish.Listen(ip, port, function (fd, addr)
		inst[method](inst, fd, addr)
	end)

	if acceptor then
		local ctx = setmetatable({}, {__index = meta})
		ctx.acceptor_ = acceptor
		acceptorCtx_[acceptor] = ctx
		return ctx
	end
end

function _M.Connect(ip, port)
	local session = co.GenSession()

	local connector = fish.Connect(ip, port, function (fd, reason)
		co.Wakeup(fd, reason)
	end)

	if not connector then
		return false, "error"
	end

	local fd, reason = co.Wait(session)
	if not fd then
		return false, reason
	end

	return fd
end

function _M.Bind(fd, header, inst, onData, onClose, onError)
	local channel = fish.Bind(fd, header, OnData, OnClose, OnError)
	channelCtx_[channel] = {
		inst = inst,
		onData = onData,
		onClose = onClose,
		onError = onError,
	}
	return channel
end

