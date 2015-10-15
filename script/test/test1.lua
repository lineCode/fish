local fish = require "lualib.fish"
local stream = require "lualib.stream"
local util = require "lualib.util"
local cjson = require "cjson"
local serialize = require "serialize"

fish.Start(function ()
	
	local time1 = fish.Timestamp()
	local inst = stream:New()
	inst:WriteString("single")
	inst:WriteBool(true)
	inst:WriteString("name")
	inst:WriteString("mrq")
	inst:WriteString("age")
	inst:WriteInt32(26)
	inst:WriteString("test1")
	inst:WriteFloat(123.456)
	inst:WriteString("test2")
	inst:WriteDouble(456.789)
	inst:WriteString("test3")
	inst:WriteInt32(64)
	for i = 1,1024*1024 do
		local data = inst:Pack()
	end
	
	local time2 = fish.Timestamp()

	print(fish.Timestamp2Second(time2-time1))

	local tbl = {single = true,name = "mrq",age = 26,test1 = 123.456,test2=456.789,test3=64}
	for i =1 ,1024*1024 do
		cjson.encode(tbl)
	end

	local time3 = fish.Timestamp()
	print(fish.Timestamp2Second(time3-time2))

end)