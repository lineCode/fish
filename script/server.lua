local co = require "lib.co"
local timer = require "lib.timer"
local common = require "lib.common"
local import = require "lib.import"


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
