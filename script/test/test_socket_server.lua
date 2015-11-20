local fish = require "lualib.fish"
local socket = require "lualib.socket"
local mongo = require "lualib.mongo"
local util = require "lualib.util"
local parser = require "HttpParser"

fish.Start(function ()
	fish.Log("test socket server")

	socket.Listen("127.0.0.1",10000,function (source)
		socket.Start(source)
		while true do
			local data = socket.Read(source)
			if data == false then
				fish.Log("socket close")
				return
			end
			local form,more,left = parser.ParserRequest(data)
			util.dump_table(form)
			print(more,left)
			return
		end
	end)
end)

