local fish = require "lualib.fish"
local stream = require "lualib.stream"
local util = require "lualib.util"

fish.Start(function ()
	
	local time1 = fish.Timestamp()
	local inst = stream:New()
	inst:WriteBool(true)
	inst:WriteString("mrq")
	inst:WriteInt32(26)
	inst:WriteFloat(123.456)
	inst:WriteDouble(456.789)
	inst:WriteInt32(64)
	for i = 1,1024*1024 do
		local data = inst:Pack()
	end
	
	local time2 = fish.Timestamp()

	print(fish.Timestamp2Second(time2-time1))

	util.dump_table(stream.Unpack(data))
end)