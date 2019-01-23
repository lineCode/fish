
local co = require "lib.co"
local common = require "lib.common"

function Init(self)
	RUNTIME_LOG:ERROR("db init")
	-- for i = 1,10000 do
	-- 	db.Query("select * from user",function (result)
	-- 	--	table.print(result)	
		
	-- 	end)
	-- end

	local CREATE_TABLE_ITEM = [[
		CREATE TABLE IF NOT EXISTS `item`(
		   `itemUid` INT UNSIGNED AUTO_INCREMENT,
		   `userUid` INT UNSIGNED,
		   `cfgId` INT UNSIGNED,
		   `amount` INT UNSIGNED,
		   PRIMARY KEY ( `itemUid` ),
		   KEY `userUid` (`userUid`) USING BTREE
		)ENGINE=InnoDB DEFAULT CHARSET=utf8;
	]]

	db.Execute(CREATE_TABLE_ITEM,function (result)
		table.print(result)
	end)

	for i=1,100 do
		local sql = string.format("insert into item (userUid,cfgId,amount) values (%d,%d,%d)",userUid,math.random(1,50),math.random(1,10))
		db.Execute(sql,function (result)
			table.print(result)
		end)
	end

end

function Fina()
	RUNTIME_LOG:ERROR("db Fina")
end

function Update(now)

end
