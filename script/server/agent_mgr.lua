
local co = require "lib.co"
local common = require "lib.common"

function Init(self)
	RUNTIME_LOG:ERROR("login init")

	RPC:Connect(config.dbAddr, 0, "db")
	RPC:Listen(config.agentMgrAddr, 0, "agentMgr")
end

function Fina()
	RUNTIME_LOG:ERROR("login mgr Fina")
end

function Update(now)

end
