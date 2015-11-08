local fish = require "lualib.fish"
local util = require "lualib.util"

local function timeout()
	fish.Log("test time out")
	fish.Log(string.format("%f kb",collectgarbage("count")))
	fish.Timeout(1000,function () timeout() end)
end

fish.Start(function ()
	fish.Log("start test timer")

	fish.Timeout(1000,function ()
		timeout()
	end)

	fish.RunInMainTick(function ()
		fish.Log("main tick")
	end)
end)

