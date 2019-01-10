local rpc = Import "rpc"

function Init(self)
	print("login init")
	rpc:Listen({ip = "127.0.0.1", port = 1989}, 1, "login")
end

function Fina()
	print("login Fina")
end

function Update(now)

end