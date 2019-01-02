local fish = require "fish"
local json = require "json"
local cjson = require "cjson"
local util = require "util"

local function OnData(channel)
	print(channel:Read())
end

local function OnClose(channel)
	print(channel,"close")
end

local function OnError(channel)
	print(channel,"error")
end

function ServerInit()
	print("server init")
	local timerId = fish.StartTimer(1,1,function ()
		print("timeout",fish.Now())
		collectgarbage("collect")	
	end)

	acceptor = fish.Listen("127.0.0.1",1997,function (fd,addr)
		print(addr)
		fish.Bind(fd,0,OnData,OnClose,OnError)
	end)

	-- print("timerId",timerId)

	-- local timerId = fish.StartTimer(1,0,function ()
	-- 	print("timeout1")
	-- end)

	-- print("timerId",timerId)



	-- fish.Listen("0.0.0.0",1995,function (fd, addr) 
	-- 	print(fd,addr)
	-- end)

	-- local ptr,size = fish.Pack({a = 1,b = 2})

	-- local tbl = fish.UnPack(ptr,size)

	-- for k,v in pairs(tbl) do
	-- 	print(k,v)
	-- end

	-- fish.Connect("127.0.0.1",1996,function (...)
	-- 	print(...)
	-- end)

	-- local tbl = {
	-- 	age = 11,
	-- 	time = 1123.12,
	-- 	fukc = "uyou",
	-- 	hx = {
	-- 		name = "hx",
	-- 		age = 123,
	-- 		fff = 1.2345
	-- 	}
	-- }

	-- local count = 1024 * 100
	-- local str
	-- util.time_diff("rapidjson.encode",function ()
	-- 	for i = 1,count do
	-- 		str = json.encode(tbl)
	-- 	end
	-- end)

	-- local tbl
	-- util.time_diff("rapidjson.decode",function ()
	-- 	for i = 1,count do
	-- 		tbl = json.decode(str)
	-- 	end
	-- end)

	-- util.time_diff("cjson.encode",function ()
	-- 	for i = 1,count do
	-- 		str = cjson.encode(tbl)
	-- 	end
	-- end)

	-- util.time_diff("cjson.decode",function ()
	-- 	for  i = 1,count do
	-- 		tbl = cjson.decode(str)
	-- 	end
	-- end)
end

function ServerFina()
	print("server fina")
end

function ServerUpdate(now)
	--print(now)
end
