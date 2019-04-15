local co = require "lib.co"

function Init(self)
	RUNTIME_LOG:ERROR("login init")
	RPC:Listen(config.login, 1, "login")

	while true do
		co.Sleep(0.1)
		for i = 1,10 do
			RUNTIME_LOG:ERROR("FUCK")
		end
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
