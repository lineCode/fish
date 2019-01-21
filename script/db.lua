
local co = require "lib.co"
local common = require "lib.common"

function Init(self)
	RUNTIME_LOG:ERROR("db init")
	db.Query("select * from user",function (result)
		table.print(result)	
		
	end)
end

function Fina()
	RUNTIME_LOG:ERROR("db Fina")
end

function Update(now)

end
