local common = require "common"
local import = require "import"
local co = require "co"
local timer = require "timer"

_G.Import = import.Import

table.print = common.Dump

local ServerInst = nil

function ServerInit(boot)
	ServerInst = Import(boot)
	ServerInst:Init()
	fish.Log("runtime", string.format("server init:%s", boot))
end

function ServerFina()
	ServerInst:Fina()
	fish.Log("runtime", "server fina")
end

function ServerUpdate(now)
	ServerInst:Update(now)
end
