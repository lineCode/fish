require("lfs")

local util = {}

local function get_type_first_print( t )
    local str = type(t)
    return string.upper(string.sub(str, 1, 1))..":"
end

function util.dump_table(t, prefix, indent_input,print)
    local indent = indent_input
    if indent_input == nil then 
        indent = 1 
    end

    if print == nil then
        print = _G["print"]
    end

    local p = nil

    local formatting = string.rep("    ", indent)
    if prefix ~= nil then
        formatting = prefix .. formatting
    end

    if t == nil then
        print(formatting.."nil")
        return
    end

    if type(t) ~= "table" then
        print(formatting..get_type_first_print(t)..tostring(t))
        return
    end

    for k,v in pairs(t) do
        local str_k = get_type_first_print(k)
        if type(v) == "table" then
            
            print(formatting..str_k..k.." -> ")

            util.dump_table(v, prefix, indent + 1,print)
        else
            print(formatting..str_k..k.." -> ".. get_type_first_print(v)..tostring(v))
        end
    end
end

--保存无环table，n为空格数可为2
function util.serialize(o, n)
    if type(o) == "number" then
            io.write(o)
    elseif type(o) == "boolean" then
            io.write((o and "true") or "false")
    elseif type(o) == "string" then
            io.write(string.format("%q", o))
    elseif type(o) == "table" then
            io.write("{\n")
            for k,v in pairs(o) do
                    io.write(string.rep("   ", n) .. "[")
                    util.serialize(k, n + 1)
                    io.write("] = ")
                    util.serialize(v, n + 1)
                    io.write(",\n")
            end
    io.write(string.rep("   ", n - 1) .. "}")
    else
            error("cannot serialize a " .. type(o))
    end
end


local function base_text(o)
    if type(o) == "number" then
        return 0, tostring(o)
    elseif type(o) == "boolean" then
        return 0, (o and "true") or "false"
    elseif type(o) == "string" then
        return 0, string.format("%q", o)
    elseif type(o) == "function" then
        return 0, tostring(o) 
    elseif type(o) == "userdata" then
        return 0, tostring(o)         
    elseif type(o) == "table" then
        return 1, tostring(o)   
    else
        return -1, "unknow"
    end
end

--保存有环的table
--@param obj 保存对象
--@param record 已保存的table记录，用于同时打印多个table时可以查看相互的关系
--@param name 保存名称，不传默认table地址，简单使用时可以用变量名
--[[使用例子
local t1 = { x = 1, y = true, z = nil }
local t2 = { y = 1 }
t1.t = t2
t1[t2] = t1

util.text(t2)
util.text(t1)
local record = {}
util.text(t2, record, "t2")
util.text(t1, record, "t1")
--]]
function util.text(obj, record, name)
    record = record or {}       --初始化
    name = name or tostring(obj)
    io.write(name, " = ")
    local ret, str = base_text(obj)
    if ret == 0 then            --基础类型
        io.write(str, "\n")
    elseif ret == 1 then        --table类型
        if record[str] then         --已经存在的table，直接保存记录名称，避免死循环
            io.write(record[str], "\n")
        else
            record[str] = name            
            io.write("{}\n")
            for k,v in pairs(obj) do
                local ret_sub, str_sub = base_text(k)
                if record[str_sub] ~= nil then
                    str_sub = record[str_sub]
                end
                local name_sub = string.format("%s[%s]", name, str_sub)
                util.text(v, record, name_sub)
            end
        end    
    else                        --未知类型
        error("cannot text a " .. type(obj))
    end
end


local function get_suffix(filename)
    return filename:match(".+%.(%w+)$")
end

function util.find_dir_files(r_table,path,suffix,is_path_name,recursive)
    for file in lfs.dir(path) do
        if file ~= "." and file ~= ".." then
            local f = path..'/'..file

            local attr = lfs.attributes (f)
            if type(attr) == "table" and attr.mode == "directory" and recursive then
                util.find_dir_files(r_table, f, suffix, is_path_name, recursive)
            else
                local target = file
                if is_path_name then target = f end

                if suffix == nil or suffix == "" or suffix == get_suffix(f) then
                    table.insert(r_table, target)
                end
            end
        end
    end
end

--追加日志到文本，默认位置Skynet文件夹
function util.AppendFile(file, str)
    local log_file = io.open(file, "a+")
    if log_file == nil then
        os.execute("mkdir "..file)
        log_file = assert(io.open(file,"a+"))
    end
    log_file:write(str)
    log_file:write("\r\n")
    log_file:close()
end

--table里元素的个数
function util.TableCount( tb )
    if type(tb) ~= type(table) then
        return 0
    end
    local count = 0
    for k,v in pairs(tb) do
        count = count + 1
    end
    return count
end

function util.TableEmpty( tb )
    if tb == nil then
        return true
    end
    if type(tb) ~= type(table) then
        print("TableEmpty error type", type(tb))
        return true
    end
    return _G.next( tb ) == nil
end

--四舍五入到整数，与客户端保持一致，支持小数3位精度
function util.round( val )
    local i, f = math.modf(val)
    if f >= 0.4999999 then
        return i + 1
    else
        return i
    end
end

--设置随机种子，buff才需要设置，其他随机不设置
function util.Seed(seed)
    random.seed(seed)
    --print(string.format("real_seed:%s", real_seed))  
end

function util.Rand(max)
    return random.rand(max)
end

--per在0到10000之间
function util.Prob(per)
    local rand = random.rand(10000)
    --print(string.format("prob:%s rand:%s", per, rand))    
    return rand < per
end

function util.ProbArray(array)
    if array == nil then
        return -1
    end
    if type(array) ~= type(table) then
        print("ProbArray error type", type(array))
        return -1
    end
    local index = math.random(1, #array)
    return array[index]
end

--战斗随机
local timeSeed = 0
local realSeed = 0
function util.TimeSeed(seed)
    timeSeed = seed
    return timeSeed
end

function util.RealSeed(id, act_id)
    realSeed = timeSeed + id + act_id
    random.fight_seed(realSeed)
    --print(string.format("realSeed:%s", realSeed))  
end

function util.FightProb(per)
    local rand = random.fight_rand(10000)
    --print(string.format("prob:%s %s", rand, per))    
    return rand < per
end
--战斗随机

--选择ValueList中满足条件的第一个
function util.SelectList(val, list)
    --最大值写在前面
    for i,v in ipairs(list.n) do
        if val >= v then
            return list.val[i]
        end
    end
    return 0        
end

function util.SelectLists(need, val, lists)    
    --最大值写在前面
    for i,list in ipairs(lists) do
        if need >= list.need then
            return util.SelectList(val, list)
        end
    end
    return 0        
end

--------复制表，深度复制，避免带环的深拷贝实现----------------------------
function util.deep_copy(object)
  local lookup_table = {}
  local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end

        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)  --注意这里，如果index也是个table的话，连index都会被拷贝出新的
        end

        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end

--十进制右边数起第b位
function util.DecimalBit(value,b)
    return math.modf((value % (10^b)) / (10^(b-1)))
end

--十进制右边数起第from到to位的数字
function util.SubDecimal(value,from,to)
    local result = 0
    for i=from,to do
        local var = util.DecimalBit(value,i)
        result = result + var * 10^(i-from)
    end
    return result
end

function util.Time2UnixTime(now,ftime)
    local zeroTime = time.next_midnight(now)
    local hourStr,minStr = string.match(ftime,"(%d+):(%d+)")
    local hour = tonumber(hourStr)
    local min = tonumber(minStr)

    return zeroTime - (24 * 3600 - hour * 3600 - min * 60)
end

return util
