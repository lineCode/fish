local co = require "lib.co"
local timer = require "lib.timer"
local common = require "lib.common"
local import = require "lib.import"
local log = require "lib.log"

table.print = common.Dump

_G.Import = import.Import
_G.config = json.load("./config")
_G.RPC = Import "rpc"
_G.RUNTIME_LOG = log:Create("runtime")


local ServerInst = nil

function ServerInit(boot)
	ServerInst = Import(boot)
	ServerInst:Init()

	RUNTIME_LOG:INFO_FM("server init:%s", boot)
end

function ServerFina()
	ServerInst:Fina()

	RUNTIME_LOG:INFO_FM("server fina")
end

function ServerUpdate(now)
	ServerInst:Update(now)
end
