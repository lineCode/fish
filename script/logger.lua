local common = require "lib.common"
local telnet = require "lib.telnet"
local socket = require "lib.socket"


function OnAccept(self, fd, addr)
	socket.Bind(fd, 2, self, "OnData", "OnClose", "OnClose")
end

function OnData(self, channel, data, size)
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
	fish.WriteLog(file, message.source or "?", message.line or -1, message.level, message.time, content)
end

function OnClose(self, channel)

end

function Init(self)
	print("logger init")
	socket.Listen(config.logger.addr, self, "OnAccept")
end

function Fina(self)
	print("logger Fina")
end

function Update(self, now)

end

function Stop(self)
	fish.Stop()
end
