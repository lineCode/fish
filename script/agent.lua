local rpc = Import "rpc.lua"

function Init(self)
	print("agent init")
	rpc:Connect({ip = "127.0.0.1", port = 1989}, 2, "agent")
end

function Fina()
	print("agent Fina")
end

function Update(now)

end
