local socket = require "lualib.socket"

local readbytes = socket.Read
local writebytes = socket.Send

local sockethelper = {}
local socket_error = setmetatable({} , { __tostring = function() return "[Socket Error]" end })

sockethelper.socket_error = socket_error

function sockethelper.readfunc(fd)
	return function (sz)
		local ret = readbytes(fd, sz)
		if ret then
			return ret
		else
			error(tostring(socket_error))
		end
	end
end

function sockethelper.writefunc(fd)
	return function(content)
		local ok = writebytes(fd, content)
		if not ok then
			error(tostring(socket_error))
		end
	end
end

return sockethelper
