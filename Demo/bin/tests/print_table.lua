
-- Показать содержание таблицы
local function print_table(tbl, tbl_name, width, noinfo)
    if tbl_name == nil then tbl_name = '.' end
    
    local getinf = debug.getinfo
    width = width or (40-4)
    local w = #tbl_name
    local h = ('-'):rep((width - w)/2)
    print( h..'[ '..tbl_name..' ]'..(w%2==0 and h or (h:sub(0,#h-1))) )
    
    if not tbl then print('nil'); return end
    for fields, value in pairs(tbl) do
        if type(fields)=='string' then fields = "'"..fields.."'" end
        if type(value) == "table" then
            -- поможет в случае _G
            if tbl ~= value then
                print("+", tbl_name.."["..fields.."] =", value)
                print_table(value, tbl_name.."["..fields.."]", width, noinfo)
            else
                print(">", tbl_name.."["..fields.."] =", value, 'циклическая ссылка')
            end
        elseif not noinfo and type(value) == 'function'  then
            local info = getinf(value)
            if info.what == 'Lua' then
                print("-", tbl_name.."["..tostring(fields).."] =", value)
                print(info.source..':'..info.linedefined..': перейти')
            else
                print("-", tbl_name.."["..tostring(fields).."] =", value, 'C-функция, исходник недоступен')
            end
        else
            print("-", tbl_name.."["..tostring(fields).."] =", value)
        end
    end
end

return print_table