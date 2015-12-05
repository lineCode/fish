local fish = require "lualib.fish"
local socket = require "lualib.socket"
local mongo = require "lualib.mongo"
local util = require "lualib.util"
local stream = require "lualib.stream"
local json = require "cjson"
local zeropack = require "Zeropack"

local function loadConfig()
	local f = assert(io.open("config" , "rb"))
	local buffer = f:read "*a"
	return json.decode(buffer)
end

local function packMessage(cmd,message)
	local size = #message+4
	assert(size <= 6*1024 , string.format("message length :%d too long",size))
	local high = math.modf(size/256)
	local low = size - high * 256

	print(size)
	local cmdhigh = math.modf(cmd/256)
	local cmdlow = cmd - cmdhigh * 256
	return  string.char(high)..string.char(low)..string.char(cmdhigh)..string.char(cmdlow) .. message
end

fish.Start(function ()
	fish.Log("test client")
	local conf = loadConfig()

	local fd = socket.Connect(conf.client.ip,conf.client.port)
	socket.Start(fd)

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

	socket.Send(fd,packMessage(1,data))
	
	socket.Close(fd)
	fish.Log("close ")
end)

