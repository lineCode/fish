local pto = require "pto"
local trie = require "trie"

local function GetTag( t )
    local str = type(t)
    return string.sub(str, 1, 1)..":"
end

function Dump(data, prefix, depth, output, record)
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
        output(tab..GetTag(data)..tostring(data))
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
        local str_k = GetTag(k)
        if type(v) == "table" then
            output(tab..str_k..tostring(k).." -> ")
            Dump(v, prefix, depth + 1, output, record)
        else
            output(tab..str_k..tostring(k).." -> ".. GetTag(v)..tostring(v))
        end
        count = count + 1
    end

    if count == 0 then
        output(tab.." {}")
    end
end

local PDemo = {
     fields = {
    {
        type = 1,
        array = true,
        name = "a"
    },
    {
        type = 5,
        array = false,
        name = "b"
    },
}
}

local PTest = {
	fields = {
    	{
    		type = 1,
    		array = true,
    		name = "a"
    	},
    	{
    		type = 5,
    		array = false,
    		name = "b"
    	},
        {
    	   type = 6,
           array = false,
           name = "c",
           fields = PDemo.fields
    	}
    }   
}


local ctx = pto.new()

ctx:import(1, "test", PTest)

local str = ctx:encode(1,{a = {1,1,2,ddd = "11"}, b = "mrq",c = {a = {{},4},b = "hx"}})

local result = ctx:decode(1, str)

Dump(result)


local ctx = trie.create()

Dump(trie.split_utf8("我是"))