local fish = require "lualib.fish"
local telnet = require "lualib.telnet.telnet"


local cmd = {}

function cmd.test(...)
	print(...)
end

function cmd.stop()
	fish.Stop()
end

fish.Start(function ()
	fish.Log("test telnet")

	telnet.Start("127.0.0.1",1990,cmd)

end)

