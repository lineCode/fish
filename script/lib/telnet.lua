local common = require "lib.common"
local socket = require "lib.socket"

local inst_ = nil
local channelData_ = setmetatable({}, {__mode = "k"})
local telnet = {}

function telnet:OnAccept(fd, addr)
	local channel = fish.Bind(fd, 0, telnet, "OnData", "OnClose", "OnError")
	channelData_[channel] = ""
end

function telnet:OnData(channel)
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

function telnet:OnClose(channel)

end

function telnet:OnError(channel)
	
end

local _M = {}

function _M.Listen(ip, port, inst)
	inst_ = inst
	socket.Listen({ip = ip, port = port}, telnet, "OnAccept")
end

return _M
