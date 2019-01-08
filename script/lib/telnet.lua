local common = require "common"

local _M = {}

acceptor_ = nil
inst_ = nil
channelData_ = setmetatable({}, {__mode = "k"})

local function OnData(channel)
	local data = channelData_[channel]
	data = data..channel:Read()
	local start, over, str = data:find("(.+)\r\n")
	if start then
		local list = common.Split(str," ")
		local method = list[1]
		local func = inst_[method]
		if func then
			func(inst_,table.unpack(list,2))
		end
		data = data:sub(over+1)
		channelData_[channel] = data
	else
		channelData_[channel] = data
	end
end

local function OnClose(channel)

end

local function OnError(channel)

end

local function OnAccept(fd, addr)
	local channel = fish.Bind(fd, 0, OnData, OnClose, OnError)
	channelData_[channel] = ""
end

function _M.Listen(ip, port, inst)
	assert(not acceptor_)

	acceptor_ = fish.Listen(ip, port, OnAccept)
	if not acceptor_ then
		error(string.format("telnet listen:%s:%d error", ip, port))
	end
	inst_ = inst
	return true
end

return _M
