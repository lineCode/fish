local util = require "util"
local import = require "import"

_G.Import = import.Import

local ServerInst = nil

function ServerInit(boot)
	fish.Log("runtime", string.format("server init:%s", boot))
	ServerInst = Import(boot)
	ServerInst:Init()
end

function ServerFina()
	ServerInst:Fina()
end

function ServerUpdate(now)
	ServerInst:Update(now)
end
