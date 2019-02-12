local co = require "lib.co"

function Init(self)
	RUNTIME_LOG:ERROR("login init")
	RPC:Listen({ip = config.loginAddr.ip, port = config.loginAddr.port}, 1, "login")

	while true do
		co.Sleep(1)
		RUNTIME_LOG:ERROR("FUCK")
		print(lfs.attributes("./runtime.log").change)
	end
end

function Fina()
	RUNTIME_LOG:ERROR("login Fina")
end

function Update(now)

end

function Fuck(self,args)
	co.Sleep(1)
	return {a = 1, b = 2}
end
