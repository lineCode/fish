local fish = require "fish"
local json = require "json"
local util = require "util"
local import = require "import"

_G.Import = import.Import

local boot = ...

local bootInst = Import(boot)

function ServerInit()
	bootInst:Init()
end

function ServerFina()
	bootInst:Fina()
end

function ServerUpdate(now)
	bootInst:Update(now)
end
