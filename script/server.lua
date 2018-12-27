local fish = require "fish"
local json = require "json"
local util = require "util"
function serverInit()
	print("server init")
	fish.StartTimer(function ()
		print("timeout")
	end,1,1)

	fish.Listen("0.0.0.0",1995,function (fd, addr) 
		print(fd,addr)
	end)

	local ptr,size = fish.Pack({a = 1,b = 2})

	local tbl = fish.UnPack(ptr,size)

	for k,v in pairs(tbl) do
		print(k,v)
	end

	print(json.encode(tbl))

	fish.Connect("127.0.0.1",1996,function (...)
		print(...)
	end)

end

function serverFina()
	print("server fina")
end

function serverUpdate(now)
	--print(now)
end
