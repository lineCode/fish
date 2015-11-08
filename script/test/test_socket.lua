local fish = require "lualib.fish"
local socket = require "lualib.socket"
local mongo = require "lualib.mongo"
local util = require "lualib.util"

local _sendBuffer = {}

local function doSend()
	for fd,bufferlist in pairs(_sendBuffer) do
		local content = table.concat(bufferlist,"")
		socket.Send(fd,content)
	end
	_sendBuffer = {}
end

fish.Start(function ()
	fish.Log("test socket")

	fish.RunInMainTick(doSend)

	local fd = assert(socket.Connect("192.168.15.246",1990))
	socket.Start(fd)
	for i = 1,10000 do
		
		local buffer = _sendBuffer[fd]
		if buffer == nil then
			buffer = {}
			_sendBuffer[fd] = buffer
		end
		table.insert(buffer,"mrq.1989.1103.2109")
	end

	while true do

		local data = socket.Read(fd)
		if data then
			print(data)
		else
			print("close")
			return
		end
	end
	
end)

