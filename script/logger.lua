local common = require "lib.common"
local telnet = require "lib.telnet"
local socket = require "lib.socket"

local logger = {}

function logger:OnAccept(fd, addr)
	fish.Bind(fd, 2, logger, "OnData", "OnClose", "OnError")
end

function logger:OnData(channel, data, size)
	local file, log = util.ParseLoggerMessage(data, size)
	fish.Log(file, log)
end

function logger:OnClose(channel)

end

function logger:OnError(channel)

end

function Init(self)
	print("logger init")
	socket.Listen({ip = "127.0.0.1", port = 1989}, logger, "OnAccept")
end

function Fina()
	print("logger Fina")
end

function Update(now)

end
