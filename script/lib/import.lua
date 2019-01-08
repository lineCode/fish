local util = require "util"

local _M = {}

local _ScriptCtx = {}

local function env_pairs(self,key)
	local key,value = next(self,key)
	if key == "__name" or key == "__timer" then
		return next(self,key)
	end
	return key,value
end

function _M.Import(file)
	local ctx = _ScriptCtx[file]
	if ctx then
		return ctx.env
	end

	local ctx = {}

	ctx.env = setmetatable({},{__index = _G,__pairs = function (self) return env_pairs,self end})

	local loader,err = loadfile(file, "text", ctx.env)
	if not loader then
		error(err)
	end
	loader()
	if ctx.env["__init__"] then
		ctx.env["__init__"](ctx.env)
	end

	_ScriptCtx[file] = ctx
	
	return ctx.env
end

function _M.Reload(file)
	local ctx = _ScriptCtx[file]
	if not ctx then
		return
	end

	loadfile(file, nil, ctx.env)

	if ctx.env["__init__"] then
		ctx.env["__init__"](ctx.env)
	end

	if ctx.env["__reload__"] then
		ctx.env["__reload__"](ctx.env)
	end
end

function _M.GetScriptCtx()
	return _ScriptCtx
end

function _M.GetModule(file)
	local ctx = _ScriptCtx[file]
	if ctx then
		return ctx.env
	end
end


return _M
