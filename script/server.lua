local co = require "lib.co"
local timer = require "lib.timer"
local common = require "lib.common"
local import = require "lib.import"

table.print = common.Dump

_G.Import = import.Import
_G.config = json.load("./config")
_G.RPC = Import "rpc"


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
