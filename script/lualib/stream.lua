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


local _M = {}

function _M:New()
	local obj = setmetatable({},{__index = self})
	obj.data = {}
	return obj
end

function _M:WriteBool(v)
	table.insert(self.data,TYPE_BOOL)
	table.insert(self.data,v)
end

function _M:WriteUInt8(v)
	table.insert(self.data,TYPE_UINT8)
	table.insert(self.data,v)
end

function _M:WriteUInt16(v)
	table.insert(self.data,TYPE_UINT16)
	table.insert(self.data,v)
end

function _M:WriteUInt32(v)
	table.insert(self.data,TYPE_UINT32)
	table.insert(self.data,v)
end

function _M:WriteUInt64(v)
	table.insert(self.data,TYPE_UINT64)
	table.insert(self.data,v)
end

function _M:WriteInt8(v)
	table.insert(self.data,TYPE_INT8)
	table.insert(self.data,v)
end

function _M:WriteInt16(v)
	table.insert(self.data,TYPE_INT16)
	table.insert(self.data,v)
end

function _M:WriteInt32(v)
	table.insert(self.data,TYPE_INT32)
	table.insert(self.data,v)
end

function _M:WriteInt64(v)
	table.insert(self.data,TYPE_INT64)
	table.insert(self.data,v)
end

function _M:WriteFloat(v)
	table.insert(self.data,TYPE_FLOAT)
	table.insert(self.data,v)
end
function _M:WriteDouble(v)
	table.insert(self.data,TYPE_DOUBLE)
	table.insert(self.data,v)
end
function _M:WriteString(str)
	table.insert(self.data,TYPE_STRING)
	table.insert(self.data,str)
end

function _M:Pack()
	return writerCore.Write(table.unpack(self.data))
end

function _M:Unpack(...)
	return readerCore.Read(...)
end

return _M