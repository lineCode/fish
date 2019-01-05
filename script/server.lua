local fish = require "fish"
local json = require "json"
local util = require "util"
local import = require "import"

_G.Import = import.Import

local ServerInst = nil

function ServerInit(boot)
	ServerInst = Import(boot)
	ServerInst:Init()
end

function ServerFina()
	ServerInst:Fina()
end

function ServerUpdate(now)
	ServerInst:Update(now)
end
