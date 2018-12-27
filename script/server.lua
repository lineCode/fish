local fish = require "fish"
local json = require "json"
local cjson = require "cjson"
local util = require "util"
function serverInit()
	print("server init")
	local timerId = fish.Timer(1,1,function ()
		print("timeout")
	end)

	print("timerId",timerId)

	local timerId = fish.Timer(1,0,function ()
		print("timeout1")
	end)

	print("timerId",timerId)



	fish.Listen("0.0.0.0",1995,function (fd, addr) 
		print(fd,addr)
	end)

	local ptr,size = fish.Pack({a = 1,b = 2})

	local tbl = fish.UnPack(ptr,size)

	for k,v in pairs(tbl) do
		print(k,v)
	end

	fish.Connect("127.0.0.1",1996,function (...)
		print(...)
	end)

	local tbl = {
		age = 11,
		time = 1123.12,
		fukc = "uyou",
		hx = {
			name = "hx",
			age = 123,
			fff = 1.2345
		}
	}

	local count = 1
	local str
	util.time_diff("rapidjson",function ()
		for i = 1,count do
			str = json.encode(tbl)
			print(str)
		end
	end)

	util.time_diff("cjson",function ()
		for i = 1,count do
			str = cjson.encode(tbl)
			print(str)
		end
	end)

	util.time_diff("rapidjson",function ()
		for i = 1,count do
			local tbl = json.decode(str)
			util.dump(tbl)
		end
	end)

	util.time_diff("cjson",function ()
		for  i = 1,count do
			local tbl = cjson.decode(str)
			util.dump(tbl)
		end
	end)

end

function serverFina()
	print("server fina")
end

function serverUpdate(now)
	--print(now)
end
