local fish = require "fish"

function serverInit()
	print("server init")
	fish.StartTimer(function ()
		print("timeout")
	end,1,1)
end

function serverFina()
	print("server fina")
end

function serverUpdate(now)
	--print(now)
end
