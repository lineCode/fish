local common = require "lib.common"
local telnet = require "lib.telnet"
local socket = require "lib.socket"

local logger = {}

function logger:OnAccept(fd, addr)
	socket.Bind(fd, 2, logger, "OnData", "OnClose", "OnError")
end

function logger:OnData(channel, data, size)
	local file, content, sz = util.ParseLoggerMessage(data, size)
	if not file then
		return
	end

	local message = fish.UnPack(content, sz)

	local fm = message.fm
	if fm then
		content = string.format(fm, table.unpack(message.log))
	else
		content = table.concat(message.log, "\t")
	end
	fish.WriteLog(file, message.source or "?", message.line or 0, message.level, message.time, content)
end

function logger:OnClose(channel)

end

function logger:OnError(channel)

end

function Init(self)
	print("logger init")
	socket.Listen(config.logger.addr, logger, "OnAccept")

	-- common.TimeDiff("test",function ()
	-- 	for i = 1, 1024*100 do
	-- 		RUNTIME_LOG:ERROR("a b c fuck,asdfasdf2sdfdsfdsf")
	-- 	end
	-- end)
	
end

function Fina()
	print("logger Fina")
end

function Update(now)

end
