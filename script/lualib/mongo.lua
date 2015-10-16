local fish = require "lualib.fish"
local util = require "lualib.util"
local Core = require "Core"
local bson = require "bson"
local Mongo = require "MongoCore"

local _M = {}

local _replyCo = {}

local function dispatch(source,_,session,docs)
	local result = {}
	if #docs == 1 then
		result = bson.decode(docs[1])
	else
		result = {}
		for _,doc in pairs(docs) do
			table.insert(result,bson.decode(doc))
		end
	end

	local reply = _replyCo[session]
	if reply == nil then
		error(string.format("error mongo session:%d",session))
	end
	reply.result = result
	fish.Wakeup(reply.co)
end

function _M.RunCommand(func,name,cmd,cmdv,...)
	local bsonCmd
	if not cmdv then
		bsonCmd = bson.encode_order(cmd,1)
	else
		bsonCmd = bson.encode_order(cmd,cmdv,...)
	end

	local session = Mongo.RunCommand(name..".$cmd",bsonCmd)
	replyFuncs[session] = func
	
	fish.Wait()

	local result = _replyCo[session].result
	_replyCo[session] = nil
	return result
end

function _M.FindOne(name,query,selector)
	query = query or {}
	selector = selector or {}

	local session = Mongo.Query(name,bson.encode(query),bson.encode(selector),0,0,1)
	_replyCo[session] = {co = coroutine.running(),result = nil}

	fish.Wait()

	local result = _replyCo[session].result
	_replyCo[session] = nil
	return result
end

function _M.FindAll(name,query,selector)
	query = query or {}
	selector = selector or {}

	local session = Mongo.Query(name,bson.encode(query),bson.encode(selector),0,0,10000)
	_replyCo[session] = {co = coroutine.running(),result = nil}

	fish.Wait()

	local result = _replyCo[session].result
	_replyCo[session] = nil
	return result
end

function _M.Insert(name,documents)
	Mongo.Insert(name,0,bson.encode(documents))
end

function _M.Update(name,selector,updator,upsert,multi)
	local flags	= (upsert and 1	or 0) +	(multi and 2 or	0)
	Mongo.Update(name,flags,bson.encode(selector),bson.encode(updator))
end

function _M.Clear()
	local tbl = {}
	for k,v in pairs(_replyCo) do
		tbl[k] = v
	end
	_replyCo = tbl
end

fish.Dispatcher("Mongo",dispatch)

return _M