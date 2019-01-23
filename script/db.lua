
local co = require "lib.co"
local common = require "lib.common"

function Init(self)
	RUNTIME_LOG:ERROR("db init")
	for i = 1,10000 do
		db.Query("select * from user",function (result)
		--	table.print(result)	
		
		end)
	end
end

function Fina()
	RUNTIME_LOG:ERROR("db Fina")
end

function Update(now)

end
