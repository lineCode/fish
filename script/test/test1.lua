local fish = require "lualib.fish"
local stream = require "lualib.stream"
local util = require "lualib.util"

fish.Start(function ()
	local inst = stream:New()

	inst:WriteBool(true)
	inst:WriteString("mrq")
	inst:WriteInt32(26)
	inst:WriteFloat(123.456)
	inst:WriteDouble(456.789)
	inst:WriteInt32(64)
	for i=1,5 do
		inst:WriteString("mrq"..i)
	end

	local data = inst:Pack()
	util.dump_table(stream.Unpack(data))
end)