local util = require "util"


local kLOG_LV_ERROR 	= 3
local kLOG_LV_WARN 		= 2
local kLOG_LV_INFO 		= 1
local kLOG_LV_DEBUG 	= 0

local eLOG_TAG = {
	[kLOG_LV_ERROR] 	= "E",
	[kLOG_LV_WARN] 		= "W",
	[kLOG_LV_INFO] 		= "I",
	[kLOG_LV_DEBUG] 	= "D",
}

local loggerLevel_ = config.loggerLevel or kLOG_LV_DEBUG

local loggerCtx = {}

local _M = {}

function _M:create(logName,depth)
	logName = logName or "unknown"
	depth = depth or 4

	local logger = loggerCtx[logName]
	if logger then
		return logger
	end

	local ctx = setmetatable({},{__index = self})
	ctx.logLevel = env.logLevel or kLOG_LV_DEBUG
	ctx.logName = logName
	ctx.depth = depth

	loggerCtx[logName] = ctx

	return ctx
end

local function getDebugInfo(logger)
	local info = debug.getinfo(logger.depth,"lS")
	return info.source,info.currentline
end

local function appendLog(logger,fm,logLevel,...)
	local mb = {
		logLevel = logLevel,
		logTag = eLOG_TAG[logLevel],
		time = os.time(),
		fm = fm,
		log = {...},
	}
	
	if logLevel == kLOG_LV_ERROR then
		local source,line = getDebugInfo(logger)
		mb.source = source
		mb.line = line
	end

	local ptr, size = fish.Pack(mb)
	fish.SendLog(logger.logName, ptr, size)
end

function _M:DEBUG(...)
	if kLOG_LV_DEBUG < loggerLevel_ then
		return
	end
	return appendLog(self,nil,kLOG_LV_DEBUG,...)
end

function _M:DEBUG_FM(fm,...)
	if kLOG_LV_DEBUG < loggerLevel_ then
		return
	end
	return appendLog(self,fm,kLOG_LV_DEBUG,...)
end

function _M:INFO(...)
	if kLOG_LV_INFO < loggerLevel_ then
		return
	end
	return appendLog(self,nil,kLOG_LV_INFO,...)
end

function _M:INFO_FM(fm,...)
	if kLOG_LV_INFO < loggerLevel_ then
		return
	end
	return appendLog(self,fm,kLOG_LV_INFO,...)
end

function _M:WARN(...)
	if kLOG_LV_WARN < loggerLevel_ then
		return
	end
	return appendLog(self,nil,kLOG_LV_WARN,...)
end

function _M:WARN_FM(fm,...)
	if kLOG_LV_WARN < loggerLevel_ then
		return
	end
	return appendLog(self,fm,kLOG_LV_WARN,...)
end

function _M:ERROR(...)
	if kLOG_LV_ERROR < loggerLevel_ then
		return
	end
	return appendLog(self,nil,kLOG_LV_ERROR,...)
end

function _M:ERROR_FM(fm,...)
	if kLOG_LV_ERROR < loggerLevel_ then
		return
	end
	return appendLog(self,fm,kLOG_LV_ERROR,...)
end


return _M