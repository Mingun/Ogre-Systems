
package.path=package.path..';../../tests/?.lua;'
if props then
    package.path=package.path..props["SciteDefaultHome"]..'/tools/?.lua;x:/OSTests/bin/tests/?.lua'
    require "base"
    require "table2"
end
local print_table = require "print_table"

local function OnChange(old, new, ...)
    -- print_table({...})
    return print("OnChange event:", old.." => "..new)
end
function simple(...)
print('cppobject ->', cppobject)
local t = {...}

-- print_table(table)
-- print_table(getmetatable(Pair))
-- В lua:
-----------------------------------------------
-- local obj = Pair('object', 5)
local obj = Pair {              -- декларативный вариант
    key     = t[1];
    value   = t[2];
	-- Вызывается при смене свойства key
    OnChange= function() print'Великий Вопрос про Жизнь, Смерть и Все остальное' end;
}
print_table(getmetatable(obj), 'mt(obj)')
print(obj, '\n---- Pair Created ----\n')

print('obj:key() ->', obj:key())
print('obj:value() ->', obj:value())

print('\n---- Get Methods Ok ----\n')

obj:setKey('obj')
print("obj:setKey('obj') ->", obj:key())-- "obj"
obj:setValue('4')
print("obj:setValue('4') ->", obj:value())-- 4

print('\n---- Set Methods Ok----\n')

obj.second = 42
obj.OnChange = OnChange
obj.first = 'key'
obj.first = 'answer'

print("obj.first ==", obj.first)
print("obj.second ==", obj.second)
assert( obj.second == 42, "obj.second == " .. tostring(obj.second) )
assert( obj.first == 'answer', "obj.first == " .. tostring(obj.first) )
-- провалится, потому что функция для хранения копируется и адрес меняется
-- assert( obj.OnChange == OnChange, "obj.OnChange == " .. tostring(obj.OnChange) )
assert( obj.OnChange ~= OnChange, "obj.OnChange ~= " .. tostring(obj.OnChange) )

print('\n---- Properties Get/Set Ok ----\n')

print('obj[1] ==', obj[1])-- "answer"
print('obj[2] ==', obj[2])-- 42

local a = obj[1]
assert( obj.first  == a, "obj[1] == " .. tostring(a) )
a = obj[2]
assert( obj.second == a, "obj[2] == " .. tostring(a) )

print('\n---- Indexators Get Ok ----\n')

obj[1] = '(null)'
obj[2] = nil-- не сработает
print('obj[1] ==', obj[1])-- "(null)"
print('obj[2] ==', obj[2])-- 42

print('\n---- Indexators Set Ok ----\n')
--[[
for _,v in obj[4] do
    print(v)
end]]
-- print('\n---- Iterators Ok ----\n')
--[[for _,name,callable,property in Pair.api do
    print(name,callable,property)
end
print('\n---- API Factory Ok ----\n')

for _,name,callable,property in obj:api() do
    print(name,callable,property)
end
print('\n---- API Object Ok ----\n')]]

-- obj:destroy()           -- "obj destroyed."
--for _, name, call, prop in obj.api() do
--    print( 'field -> ', name, call, prop )
--end

end
if props then
    simple('call', 'from', 'SciTE')
end

--[[
local ogre = Entity {
    name = 'Ogre',
	description = '',
    mesh = 'Sinbad.mesh',
    material = 'Sinbad.material',
    scripts = {
    },
    vars = {
    },
    animations = {
    },
    sounds = {
    },
    speed = 1,
}
]]