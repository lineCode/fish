local rpc = Import "rpc"

function Init(self)
	RUNTIME_LOG:ERROR("login init")
	rpc:Listen({ip = config.loginAddr.ip, port = config.loginAddr.port}, 1, "login")
end

function Fina()
	RUNTIME_LOG:ERROR("login Fina")
end

function Update(now)

end
