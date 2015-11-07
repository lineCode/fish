local fish = require "lualib.fish"
local socket = require "lualib.socket"
local mongo = require "lualib.mongo"
local util = require "lualib.util"

fish.Start(function ()
	fish.Log("test socket")


	for i = 1,500 do
		fish.Fork(function ()
			local fd = assert(socket.Connect("127.0.0.1",10000))
			socket.Start(fd)
			socket.Send(fd,"mrq.1989.1103.2109")

			local data = socket.Read(fd)
			print(fd,data)
			local data = socket.Read(fd)
			if not data then
				print(fd,"close")
			end
		end)
	end
	-- socket.Close(fd)
end)

