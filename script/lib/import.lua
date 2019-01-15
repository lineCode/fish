local util = require "util"

local _M = {}

local _ScriptCtx = {}

local function EnvPairs(self,key)
	local key,value = next(self,key)
	if key == "__name" or key == "__timer" then
		return next(self,key)
	end
	return key,value
end

local function LoadFile(path, env)
	local loader,err = loadfile(path, "text", env)
	if not loader then
		error(err)
	end
	loader()
end

function _M.Import(file)
	local ctx = _ScriptCtx[file]
	if ctx then
		return ctx.env
	end

	local path = package.searchpath(file, package.path)

	local ctx = {}
	ctx.path = path
	ctx.env = setmetatable({},{__index = _G,__pairs = function (self) return EnvPairs,self end})
	
	LoadFile(ctx.path, ctx.env)

	if ctx.env["__init__"] then
		ctx.env["__init__"](ctx.env)
	end

	local attr = lfs.attributes(path)
	ctx.change = attr.change

	_ScriptCtx[file] = ctx
	return ctx.env
end

function _M.Reload(file)
	local ctx = _ScriptCtx[file]
	if not ctx then
		return
	end

	LoadFile(ctx.path, ctx.env)

	if ctx.env["__reload__"] then
		ctx.env["__reload__"](ctx.env)
	end
end

function _M.AutoReload()
	for file,ctx in pairs(_ScriptCtx) do
		local attr = lfs.attributes(ctx.path)
		if attr.change ~= ctx.change then
			_M.Reload(file)
		end
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
