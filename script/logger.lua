local common = require "common"

local acceptor = nil

local httpAcceptor = nil


local function OnData(channel, data, size)
	local file, log = util.ParseLoggerMessage(data, size)
	fish.Log(file, log)
end

local function OnClose(channel)
	print("channel close",channel)
end

local function OnError(channel)
	print("channel error",channel)
end

local function OnClientAccept(fd, addr)
	print("accept channel",fd,addr)
	fish.Bind(fd, 2, OnData, OnClose, OnError)
end

local function OnHttpAccept(fd, addr)
	print("accept http",addr)
	fish.BindHttp(fd, function (request)
		print(request:GetUrl())
		common.dump(request:GetHeader())
		request:Reply(200,"fuck")
		request:Close(0)	
	end)
end

function Init()
	print("logger init")
	acceptor = fish.Listen("127.0.0.1", 1989, OnClientAccept)
	httpAcceptor = fish.Listen("0.0.0.0", 2000, OnHttpAccept)

	fish.StartTimer(1,1,function ()
		print("timeout")
	end)

	table.print(common.ListDir("./"))
end

function Fina()
	print("logger Fina")
end

function Update(now)

end
