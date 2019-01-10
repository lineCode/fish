local rpc = Import "rpc"

function Init(self)
	print("login init")
	rpc:Listen({ip = "127.0.0.1", port = 3989}, 1, "login")
	print("!!!")
end

function Fina()
	print("login Fina")
end

function Update(now)

end
