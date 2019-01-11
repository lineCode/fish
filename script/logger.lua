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
		
		local tag = args.tag
		local source = args.source
		local line = args.line
		
		local log
		local fm = args.fm
		if fm then
			log = strFmt(fm,table.unpack(args.log))
		else
			log = table.concat(args.log,"\t")
		end

		local content
		if source then
			content = strFmt("[%s][%s %s:%d] %s\r\n",tag,osData("%Y-%m-%d %H:%M:%S",args.time),source,line,log)
		else
			content = strFmt("[%s][%s] %s\r\n",tag,osData("%Y-%m-%d %H:%M:%S",args.time),log)
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
