local fish = require "lualib.fish"
local socket = require "lualib.socket"

local _M = {}

local function splitLine(cmdline)
	local split = {}
	for i in string.gmatch(cmdline, "%S+") do
		table.insert(split,i)
	end
	return split
end


function _M.Start(host,port,cmdset)
	socket.Listen(host,port,function (source)
		socket.Start(source)
		local content = ""
		while true do
			local data = socket.Read(source)
			if data == false then
				fish.Log("telnet close")
				return
			end
			content = content..data

			local f,e = content:find("\n",1,true)
			if f then
				local line = content:sub(1,f-1)
				local splits = splitLine(line)

				local cmd = cmdset[splits[1]]
				local ok, list
				if cmd then
					ok, list = pcall(cmd, select(2,table.unpack(splits)))
					if ok then
						socket.Send(source,"ok")
					else
						socket.Send(source,list)
					end
					socket.Send(source, "\r\n")
				else
					socket.Send(source, string.format("cmd:%s not found",splits[1]))
					socket.Send(source, "\r\n")
				end

				content = content:sub(e+1)
			end
		end
	end)
end

return _M