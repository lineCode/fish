local co = require "lib.co"
local rpc = Import "rpc"

function Init(self)
	RUNTIME_LOG:ERROR("agent init")
	co.Fork(function ()
		assert(rpc:Connect({ip = config.loginAddr.ip, port = config.loginAddr.port}, 2, "agent", 5))
		RUNTIME_LOG:ERROR("register ok")
	end)
end

function Fina()
	RUNTIME_LOG:ERROR("agent Fina")
end

function Update(now)

end

--call by cxx
function _G.OnClientEnter(vid)
	RUNTIME_LOG:ERROR_FM("client enter:%d",vid)
end

function _G.OnClientError(vid)
	RUNTIME_LOG:ERROR_FM("client error:%d",vid)
end

function _G.OnClientData(vid, data, size)

end
