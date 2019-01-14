

function Init(self)
	RUNTIME_LOG:ERROR("login init")
	RPC:Listen({ip = config.loginAddr.ip, port = config.loginAddr.port}, 1, "login")
end

function Fina()
	RUNTIME_LOG:ERROR("login Fina")
end

function Update(now)

end

function Fuck(self,args)
	table.print(args,"Fuck")
	return {a = 1, b = 2}
end