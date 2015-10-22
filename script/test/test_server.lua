local fish = require "lualib.fish"
local socket = require "lualib.socket"
local cjson = require "cjson"
local util = require "lualib.util"

fish.Start(function ()
	fish.Log("start test")

	local serverfd = fish.StartServer("127.0.0.1",1990,
		function (source,method,data)
			fish.Log(string.format("server:get method:%d,data:%s",method,data))
			fish.Sleep(1000)
			fish.Log("server:return")
			fish.Ret(cjson.encode({yes = "copy"}))

		end,
		function (source)
			fish.Log(string.format("server:on connected:%d",source))
		end,
		function (source)
			fish.Log(string.format("server:on close:%d",source))
		end)

	local clientfd = fish.ConnectServer("127.0.0.1",1990,
		function (source,method,data)
		
		end,
		function (source)
			fish.Log(string.format("client:on connected:%d",source))
			fish.Log("client:try to call server")
			local data,size = fish.Call(source,1,cjson.encode({name = "mrq",age = 26}))
			fish.Log(string.format("client:call server return:%s",data))
			fish.Close(source)
			fish.Log("done")
		end,
		function (source)
			fish.Log(string.format("client on close:%d",source))
		end)
end)

