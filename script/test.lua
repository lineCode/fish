


function Init()
	fish.StartTimer(1,1,function ()
		for i = 1,1024 do
			fish.Log("runtime", "fuck"..math.random(1, 1024))
		end
	end)
end

function Fina()

end

function Update(now)

end