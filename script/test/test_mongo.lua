local fish = require "lualib.fish"
local mongo = require "lualib.mongo"
local util = require "lualib.util"

fish.Start(function ()
	fish.Log("tset mongo")

	-- for i = 1,10000 do
	-- 	mongo.Insert("fish.role",{id=i,name="mrq."..i})
	-- end

	-- local updator = {}
	-- updator["$set"] = {id = 1,name = "dl"}
	-- mongo.Update("fish.role",{id = 1},updator)

	-- local result = mongo.FindOne("fish.role",{id = 1})
	-- util.dump_table(result)
	
	local result = mongo.FindAll("fish.role")
	for _,info in pairs(result) do
		print(info.id,info.name)
	end
	-- fish.Stop()
end)

