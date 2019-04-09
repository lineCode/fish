
local co = require "lib.co"
local common = require "lib.common"

function Init(self)
	RUNTIME_LOG:ERROR("master init")
	RPC:Listen(config.masterAddr, 1, "master")
end

function Fina()
	RUNTIME_LOG:ERROR("master fina")
end

function Update(now)

end
