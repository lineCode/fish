local rpc = Import "rpc"

function Init(self)
	print("login init")
	rpc:Listen({ip = config.loginAddr.ip, port = config.loginAddr.port}, 1, "login")
end

function Fina()
	print("login Fina")
end

function Update(now)

end
