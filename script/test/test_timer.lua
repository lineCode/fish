local fish = require "lualib.fish"
local util = require "lualib.util"
local telnet = require "lualib.telnet"

local function timeout()
	fish.Timeout(math.random(1000,10000),function () timeout() end)
end

local cmd = {}

function cmd.mem(...)
	local kb, bytes = collectgarbage "count"
	return string.format("%dkb",kb)
end

function cmd.gc()
	collectgarbage "collect"
	local kb, bytes = collectgarbage "count"
	return string.format("%dkb",kb)
end

function cmd.luamem()
	local a,b,c = fish.MemInfo()
	return string.format("%fkb %fkb %fkb",a/1024,b/1024,c/1024)
end


function cmd.stop()
	fish.Stop()
end


fish.Start(function ()
	fish.Log("start test timer")
	telnet.Start("127.0.0.1",1990,cmd)

	for i = 1,100000 do
		fish.Timeout(math.random(1000,10000),function ()
			timeout()
		end)
	end
end)

