local common = require "lib.common"
local telnet = require "lib.telnet"
local socket = require "lib.socket"

local logger = {}

function logger:OnAccept(fd, addr)
	socket.Bind(fd, 2, logger, "OnData", "OnClose", "OnError")
end

function logger:OnData(channel, data, size)
	local source
	local line
	local level
	local time
	local content
	local type, file, source, line, level, time, content = util.ParseLoggerMessage(data, size)
	if type == 0 then
		fish.WriteLog(file, source, line, level, time, content)
	else
		local info = fish.UnPack(source, line)

		source = info.source
		line = info.line
		level = info.level
		time = info.time
		
		local fm = info.fm
		if fm then
			content = string.format(fm,table.unpack(info.log))
		else
			content = table.concat(info.log,"\t")
		end
		fish.WriteLog(file, source or "-", line or "0", level, time, content)
	end
	
end

function logger:OnClose(channel)

end

function logger:OnError(channel)

end

function Init(self)
	print("logger init")
	socket.Listen({ip = config.loggerAddr.ip, port = config.loggerAddr.port}, logger, "OnAccept")

	listener = fish.Listen({ip = "0.0.0.0", port = 1990}, function (fd,addr)
		print(fd,addr)
		fish.BindHttp(fd,function (request)
			print(request:GetUrl())
			table.print(request:GetHeader())
		end)
	end)
	print(listener)
end

function Fina()
	print("logger Fina")
end

function Update(now)

end
