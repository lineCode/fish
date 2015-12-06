local fish = require "lualib.fish"
local util = require "lualib.util"

local function timeout()
	fish.Timeout(math.random(1000,10000),function () timeout() end)
end

fish.Start(function ()
	fish.Log("start test timer")

	for i = 1,100000 do
		fish.Timeout(math.random(1000,10000),function ()
			timeout()
		end)
	end
end)

