
local co = require "lib.co"
local common = require "lib.common"

function Init(self)
	RUNTIME_LOG:ERROR("db init")
	RPC:Listen(config.dbAddr, 1, "db")
end

function Fina()
	RUNTIME_LOG:ERROR("db Fina")
end

function Update(now)

end
