local fish = require "lualib.fish"
local util = require "lualib.util"
local socket = require "lualib.socket"
local httpd = require "lualib.http.httpd"
local sockethelper = require "lualib.http.sockethelper"
local urllib = require "lualib.http.url"

local function loadConfig()
	local f = assert(io.open("config" , "rb"))
	local buffer = f:read "*a"
	return cjson.decode(buffer)
end

local function response(id, ...)
	local ok, err = httpd.write_response(sockethelper.writefunc(id), ...)
	if not ok then
		-- if err == sockethelper.socket_error , that means socket closed.
		fish.Log(string.format("fd = %d, %s", id, err))
	end
end

fish.Start(function ()
	fish.Log("start test http")
	socket.Listen("127.0.0.1",1990,function (source)
		socket.Start(source)
		local code, url, method, header, body = httpd.read_request(sockethelper.readfunc(source), 8192)
		if code then
			if code ~= 200 then
				response(source, code)
			else

				local path, query = urllib.parse(url)
				-- print(code,url,method,header,body,path,query)
				local args
				if method == "GET" then
					if query then
						args = urllib.parse_query(query)
					end
				else
					local content_type = header["content-type"]
					local l,r = content_type:match "(.*)/(.*)"
					if r == "json" then
						args = json.decode(body)
					elseif r == "x-www-form-urlencoded" then
						args = urllib.parse_query(body)
					else
						assert(false,content_type)
					end
				end

				response(source, code, "hello,i am fish")
			end
		else
			if url == sockethelper.socket_error then
				fish.Log("socket closed")
			else
				fish.Log(url)
			end
		end
		socket.Close(source)
	end)
end)

