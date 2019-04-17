coroutine.resume = costats.resume
coroutine.yield = costats.yield

local co = require "lib.co"
local timer = require "lib.timer"
local common = require "lib.common"
local import = require "lib.import"
local log = require "lib.log"


table.print = common.Dump

Import = import.Import
config = json.load("./bin/debug/config")
RPC = Import "rpc"

RUNTIME_LOG = log:Create("runtime")


local ServerInst

function ServerInit(boot)
	ServerInst = Import(boot)
	co.Fork(ServerInst.Init, ServerInst)

	RUNTIME_LOG:INFO_FM("server init:%s", boot)
end

function ServerFina()
	ServerInst:Fina()

	RUNTIME_LOG:INFO_FM("server fina")
end

function ServerUpdate(now)
	ServerInst:Update(now)
end

function ServerStop()
	ServerInst:Stop()
end
