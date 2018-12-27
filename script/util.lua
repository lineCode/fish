local fish = require "fish"

local _M = {}

local function get_tag( t )
    local str = type(t)
    return string.sub(str, 1, 1)..":"
end

function _M.dump(data, prefix, depth, output, record)
    record = record or {}

    depth = depth or 1

    if output == nil then
        output = _G.print
    end

    local tab = string.rep("\t", depth)
    if prefix ~= nil then
        tab = prefix .. tab
    end

    if data == nil then
        output(tab.." nil")
        return
    end

    if type(data) ~= "table" then
        output(tab..get_tag(data)..tostring(data))
        return
    end

    if record[data] then
        output(tab.." {}")
	return
    end
    --assert(record[data] == nil)
    record[data] = true

    local count = 0
    for k,v in pairs(data) do
        local str_k = get_tag(k)
        if type(v) == "table" then
            output(tab..str_k..tostring(k).." -> ")
            _M.dump(v, prefix, depth + 1, output, record)
        else
            output(tab..str_k..tostring(k).." -> ".. get_tag(v)..tostring(v))
        end
        count = count + 1
    end

    if count == 0 then
        output(tab.." {}")
    end
end

function _M.time_diff(desc,func)
    local now = fish.Now()
    func()
    print(string.format("%s:%f",desc,fish.Now() - now))
end

return _M