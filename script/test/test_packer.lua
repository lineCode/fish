local fish = require "lualib.fish"
local stream = require "lualib.stream"
local util = require "lualib.util"
local cjson = require "cjson"
local serialize = require "serialize"
local bson = require "bson"
local profiler = require "profiler"

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
	
	local data = inst:Pack()

	util.dump_table(stream.Unpack(data))

	
	fish.Stop()
	-- local tbl = {single = true,name = "mrq",age = 26,test1 = 123.456,test2=456.789,test3=64}
	-- for i =1 ,1024*1024 do
	-- 	cjson.encode(tbl)
	-- end

	-- local time3 = fish.Timestamp()
	-- print("cjson",fish.Timestamp2Second(time3-time2))

	-- for i =1 ,1024*1024 do
	-- 	serialize.pack(tbl)
	-- end

	-- local time4 = fish.Timestamp()
	-- print("serialize",fish.Timestamp2Second(time4-time3))

	-- for i =1 ,1024*1024 do
	-- 	bson.encode(tbl)
	-- end

	-- local time5 = fish.Timestamp()
	-- print("bson",fish.Timestamp2Second(time5-time4))

	-- for i =1 ,1024*1024 do
	-- 	bsoncpp.Encode(tbl)
	-- end

	-- local time6 = fish.Timestamp()
	-- print("bsoncpp",fish.Timestamp2Second(time6-time5))
end)