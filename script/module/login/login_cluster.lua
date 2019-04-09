




function Init(self)

end

function InitMaster(self)
	LOGIN_MASTER:Init()
end


function InitSlaver(self, args)
	LOGIN_SLAVER:Init(args)
end