

local acceptor = nil



local function OnData(channel, data, size)
	local file, log = util.ParseLoggerMessage(data, size)
	print(file,log)
end

local function OnClose(channel)

end

local function OnError(channel)

end

local function OnClientAccept(fd, addr)
	fish.Bind(fd, 2, OnData, OnClose, OnError)
end

function Init()
	print("logger init")
	acceptor = fish.Listen("127.0.0.1", 1989)
end

function Fina()
	print("logger Fina")
end

function Update(now)

end
