local co = require "lib.co"
local rpc = Import "rpc"

function Init(self)
	print("agent init")
	co.Fork(function ()
		assert(rpc:Connect({ip = "127.0.0.1", port = 3989}, 2, "agent", 5))
		print("register ok")
	end)
end

function Fina()
	print("agent Fina")
end

function Update(now)

end

--call by cxx
function _G.OnClientEnter(vid)
	print("client enter",vid)
end

function _G.OnClientError(vid)
	print("client error",vid)
end

function _G.OnClientData(vid, data, size)

end
