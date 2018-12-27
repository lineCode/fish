local fish = require "fish"

function serverInit()
	print("server init")
	fish.StartTimer(function ()
		print("timeout")
	end,1,1)

	fish.Listen("0.0.0.0",1995,function (fd, addr) 
		print(fd,addr)
	end)
end

function serverFina()
	print("server fina")
end

function serverUpdate(now)
	--print(now)
end
