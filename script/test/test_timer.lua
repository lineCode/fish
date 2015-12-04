local fish = require "lualib.fish"
local util = require "lualib.util"

local function timeout()
	fish.Log(string.format("test time out"))
	fish.Log(string.format("%f kb",collectgarbage("count")))
	fish.Timeout(1000,function () timeout() end)
end

fish.Start(function ()
	fish.Log("start test timer")

	for i = 1,10 do 
		fish.Timeout(1000,function ()
			timeout()
		end)
	end

	fish.Sleep(1000 * 3)
	fish.Stop()
end)

