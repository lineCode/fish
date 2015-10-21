local fish = require "lualib.fish"
local socket = require "lualib.socket"
local mongo = require "lualib.mongo"
local util = require "lualib.util"

fish.Start(function ()
	fish.Log("test socket")

	socket.Listen("127.0.0.1",10000,function (source)
		socket.Start(source)
		while true do
			local data = socket.Read(source,3)
			if data == false then
				fish.Log("socket close")
				return
			end

			fish.Log(string.format("socket read:%s",data))
		end
	end)

	local fd = assert(socket.Connect("127.0.0.1",10000))
	socket.Start(fd)
	socket.Send(fd,"mrq")
	socket.Close(fd)

end)

