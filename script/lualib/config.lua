local csv2table = require "lualib.csv2table"
local Core = require "Core"
local Json = require "cjson"

local setmetatable = setmetatable
local rawget = rawget
local tinsert = table.insert
local pack = table.pack
local unpack = table.unpack
local pairs = pairs


local _csvtbl = {}

local config = {}
local config_meta = {
	__index = function (self,key)
		return rawget(config,key) or self:get(key)
	end
}

local field = {}
local field_meta = {
	__index = function (self,key)
		return rawget(field,key) or self:get(key)
	end
}

function field:get(name)
	setmetatable(self,nil)
	if self.fields == nil then
		self.fields = {}
	end

	tinsert(self.fields,name)
	
	return setmetatable(self,field_meta)
end

function field:find()
	setmetatable(self,nil)
	if self.fields == nil then
		return config:find(self.name)
	else
		return config:find(self.name,unpack(self.fields))
	end
end

function config:get(name)
	local ctx = {name = name}
	return setmetatable(ctx,field_meta)
end

--host call load
function config:load(dir,file)
	local data = csv2table.load(dir,file)
	local name = string.match(file,"(.*).csv")
	_csvtbl[name] = data
end

function config:find(name,...)
	local tbl = pack(...)
	local val = _csvtbl[name]

	for i =1,tbl.n do
		val = val[tbl[i]]
		if val == nil then
			return
		end
	end
	return val
end

function config:dump2Cxx()
	for name,data in pairs(_csvtbl) do
		for id,line in pairs(data) do
			Core.ConfigInit(name,id,Json.encode(line))
		end
	end
end


return setmetatable(config,config_meta)