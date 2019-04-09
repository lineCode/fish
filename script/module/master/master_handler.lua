



loginServerMgr = loginServerMgr or {}
agentServerMgr = agentServerMgr or {}


function __init__(self)


end




function LoginRegister(self, args, channel)
	local info = {id = args.loginId, addr = args.loginAddr, channel = channel}
	loginServerMgr[args.loginId] = info
end

function ApplyLoginMaster(self, loginId)
	RPC:SendLogin(loginId, "LOGIN_CLUSTER:InitMaster")

	local masterInfo = loginServerMgr[loginId]
	local info = {id = masterInfo.id, addr = masterInfo.addr}

	for id,info in pairs(self.loginServerMgr) do
		RPC:SendLogin(id, "LOGIN_CLUSTER:InitSlaver", info)
	end
end

function AgentRegister(self, args, channel)
	local info = {id = args.agentId, addr = args.agentAddr, channel = channel}
	agentServerMgr[args.agentId] = agentAddr
end

function ApplyAgentMaster(self, agentId)
	RPC:SendAgent(agentId, "AGENT_CLUSTER:InitMaster")

	local masterInfo = agentServerMgr[agentId]
	local info = {id = masterInfo.id, addr = masterInfo.addr}

	for id,info in pairs(self.agentServerMgr) do
		RPC:SendAgent(id, "AGENT_CLUSTER:InitSlaver", info)
	end
end