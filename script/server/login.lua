
local co = require "lib.co"
local common = require "lib.common"

function Init(self)
	RUNTIME_LOG:ERROR("login init")

	RPC:Connect(config.dbAddr, 1, "db")
	RPC:Connect(config.loginMgrAddr, 2, "loginMgr")
	
	RPC:Listen(config.loginMgrAddr, 2, "loginMgr")
end

function Fina()
	RUNTIME_LOG:ERROR("login mgr Fina")
end

function Update(now)

end
