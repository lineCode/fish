local writerCore = require "MessageWriter"
local readerCore = require "MessageReader"

local TYPE_BOOL = 1
local TYPE_UINT8 = 2
local TYPE_UINT16 = 3
local TYPE_UINT32 = 4
local TYPE_UINT64 = 5
local TYPE_INT8 = 6
local TYPE_INT16 = 7
local TYPE_INT32 = 8
local TYPE_INT64 = 9
local TYPE_FLOAT = 10
local TYPE_DOUBLE = 11
local TYPE_STRING = 12

local tinsert = table.insert
local tunpack = table.unpack
local setmetatable = setmetatable
local coreWrite = writerCore.Write
local coreRead = readerCore.Read

local _M = {}

function _M:New()
	local obj = setmetatable({},{__index = self})
	obj.data = {}
	return obj
end

function _M:WriteBool(v)
	tinsert(self.data,TYPE_BOOL)
	tinsert(self.data,v)
end

function _M:WriteUInt8(v)
	tinsert(self.data,TYPE_UINT8)
	tinsert(self.data,v)
end

function _M:WriteUInt16(v)
	tinsert(self.data,TYPE_UINT16)
	tinsert(self.data,v)
end

function _M:WriteUInt32(v)
	tinsert(self.data,TYPE_UINT32)
	tinsert(self.data,v)
end

function _M:WriteUInt64(v)
	tinsert(self.data,TYPE_UINT64)
	tinsert(self.data,v)
end

function _M:WriteInt8(v)
	tinsert(self.data,TYPE_INT8)
	tinsert(self.data,v)
end

function _M:WriteInt16(v)
	tinsert(self.data,TYPE_INT16)
	tinsert(self.data,v)
end

function _M:WriteInt32(v)
	tinsert(self.data,TYPE_INT32)
	tinsert(self.data,v)
end

function _M:WriteInt64(v)
	tinsert(self.data,TYPE_INT64)
	tinsert(self.data,v)
end

function _M:WriteFloat(v)
	tinsert(self.data,TYPE_FLOAT)
	tinsert(self.data,v)
end

function _M:WriteDouble(v)
	tinsert(self.data,TYPE_DOUBLE)
	tinsert(self.data,v)
end

function _M:WriteString(str)
	tinsert(self.data,TYPE_STRING)
	tinsert(self.data,str)
end

function _M:Pack()
	return coreWrite(tunpack(self.data))
end

function _M.Unpack(...)
	return coreRead(...)
end

return _M