local common = require "lib.common"
local telnet = require "lib.telnet"
local socket = require "lib.socket"

local logger = {}

function logger:OnAccept(fd, addr)
	socket.Bind(fd, 2, logger, "OnData", "OnClose", "OnError")
end

function logger:OnData(channel, data, size)
	local type, file, data, size = util.ParseLoggerMessage(data, size)
	if type == 0 then
		fish.WriteLog(file, data)
	else
		local info = fish.UnPack(data, size)
		
		local tag = info.tag
		local source = info.source
		local line = info.line
		
		local log
		local fm = info.fm
		if fm then
			log = string.format(fm,table.unpack(info.log))
		else
			log = table.concat(info.log,"\t")
		end

		local content
		if source then
			content = string.format("[%s][%s %s:%d] %s\r\n",tag,os.date("%Y-%m-%d %H:%M:%S",info.time),source,line,log)
		else
			content = string.format("[%s][%s] %s\r\n",tag,os.date("%Y-%m-%d %H:%M:%S",info.time),log)
		end

		fish.WriteLog(file, content)
	end
	
end

function logger:OnClose(channel)

end

function logger:OnError(channel)

end

function Init(self)
	print("logger init")
	socket.Listen({ip = config.loggerAddr.ip, port = config.loggerAddr.port}, logger, "OnAccept")
end

function Fina()
	print("logger Fina")
end

function Update(now)

end
