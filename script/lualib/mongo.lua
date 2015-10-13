local fish = require "lualib.fish"
local util = require "lualib.util"
local Core = require "Core"
local Bson = require "Bson"
local Mongo = require "MongoCore"

local _M = {}

local _replyCo = {}

local function dispatch(source,_,session,docs)
	local result = {}
	if #docs == 1 then
		result = Bson.Decode(docs[1])
	else
		result = {}
		for _,doc in pairs(docs) do
			table.insert(result,Bson.Decode(doc))
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
	local bsonCmd,bsonSize
	if not cmdv then
		bsonCmd,bsonSize = Bson.EncodeOrder(cmd,1)
	else
		bsonCmd,bsonSize = Bson.EncodeOrder(cmd,cmdv,...)
	end

	local session = Mongo.RunCommand(name..".$cmd",bsonCmd,bsonSize)
	replyFuncs[session] = func
end

function _M.FindOne(name,query,selector)
	query = query or {}
	selector = selector or {}

	local queryData,querySize = Bson.Encode(query)
	local selectorData,selectorSize = Bson.Encode(selector)
	local session = Mongo.Query(name,queryData,querySize,selectorData,selectorSize,0,0,1)
	_replyCo[session] = {co = coroutine.running(),result = nil}

	fish.Wait()

	local result = _replyCo[session].result
	_replyCo[session] = nil
	return result
end

function _M.FindAll(name,query,selector)
	query = query or {}
	selector = selector or {}

	local queryData,querySize = Bson.Encode(query)
	local selectorData,selectorSize = Bson.Encode(selector)
	local session = Mongo.Query(name,queryData,querySize,selectorData,selectorSize,0,0,10000)
	_replyCo[session] = {co = coroutine.running(),result = nil}

	fish.Wait()

	local result = _replyCo[session].result
	_replyCo[session] = nil
	return result
end

function _M.Insert(name,documents)
	local doc,docSize = Bson.Encode(documents)
	Mongo.Insert(name,0,doc,docSize)
end

function _M.Update(name,selector,updator,upsert,multi)
	local flags	= (upsert and 1	or 0) +	(multi and 2 or	0)
	local selector,selectorsize = Bson.Encode(selector)
	local updator,updatorsize = Bson.Encode(updator)
	Mongo.Update(name,flags,selector,selectorsize,updator,updatorsize)
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