
local TE = require('templates')
local T  = require('types')
-- Генераторы
local G = {__doc__ = 'Генераторы кода'}

local CEvaluator    = TE.CEvaluator
local Method        = TE.Method
local Indexator     = TE.Indexator
local SimpleMethod  = TE.SimpleMethod
local Constructor   = TE.Constructor
local Evaluator     = TE.Evaluator
local Args          = TE.Args
local Property      = TE.Property
local Init          = TE.Init
local patt          = TE.patt

string.substitute = function(self, replacer)
    return self:gsub(TE.patt, replacer)
end

-- Конструирует список аргументов
-- args - список вида {'type1', 'type2', 'type3', ...}
function G.getArgs(args, indent)
    indent = indent or ''
    local res = {}
    table.foreach(args, function(k,v)
        -- v[1] - тип
        -- v[2] - имя
        local s
        if type(v) == 'table' then
            s = {
                type = T[ v[1] ] or 'ScriptableVar',
                name = v[2]
            }
        else
            s = {
                type = T[v] or 'ScriptableVar',
                name = ''
            }
        end
        res[k] = Args:substitute(s)
    end)
    return table.concat(res, indent)
end

-- Конструирует список вычислителей
-- overloads - список элементов вида {i={...},o={...}},
function G.getEvaluators(overloads, name, fnName, typeName, indent)
    indent = indent or ''
    local res = ''
    local counter = 0
    local function autocounter() counter = counter + 1 return counter end
    for _,overload in ipairs(overloads) do
        local s = {
            ['in']  = table.concat(overload.i, ','),
            ['out'] = table.concat(overload.o, ','),
            inArgs  = G.getArgs(overload.i, indent..string.rep(' ', 8)),
            outArgs = G.getArgs(overload.o, indent..string.rep(' ', 8)),
            name    = name,
            fnName  = fnName,
            num     = autocounter(),
            typeName= typeName,
        }
        local e = Evaluator:gsub('\n', '\n'..indent)
        res = res..e:substitute(s)
    end
    return res
end

-- Конструирует код инициализации вычислителей
function G.getInits(overloads, indent)
    indent = indent or ''
    local res = {}
    table.foreach(overloads, function(k,v) res[k] = Init:substitute('E'..tostring(k)) end)
    return table.concat(res, indent)
end

-- Генерирует код методов объекта
function G.generateMethods(typeName, Object)
    local indent = string.rep(' ', 4)
    local res = ''
    for name, overloads in pairs(Object.methods) do
        if #overloads == 1 then
            local s = {
                ['in']  = table.concat(overloads[1].i, ','),
                ['out'] = table.concat(overloads[1].o, ','),
                name    = name,
                fnName  = name,
                inArgs  = G.getArgs(overloads[1].i, indent:rep(2)),
                outArgs = G.getArgs(overloads[1].o, indent:rep(2)),
                typeName= typeName,
            }
            res = res..SimpleMethod:substitute(s)
        else
            local s = {
                name        = name,
                init        = G.getInits(overloads, indent),
                evaluators  = G.getEvaluators(overloads, name, name, typeName, indent),
            }
            res = res..Method:substitute(s)
        end
    end
    return res
end

-- Генерирует код сеттеров и геттеров свойств объекта
function G.generateProperties(typeName, Object)
    local indent = string.rep(' ', 4)
    local res = ''
    for name, type in pairs(Object.properties) do
        local s = {
            name    = name,
            type    = T[type] or 'ScriptableVar',
            outArg  = Args:substitute(T[type] or 'ScriptableVar'),
            out     = type,
            typeName= typeName,-- фактическое имя типа Scriptable
            fnSet   = 'set'..name:sub(1,1):upper()..name:sub(2),
            fnGet   = 'get'..name:sub(1,1):upper()..name:sub(2),
        }
        res = res..Property:substitute(s)
    end
    return res
end

-- Генерирует код индексаторов объекта
function G.generateIndexes(typeName, Object)
    local indent = ''
    local res = ''
    for _,overload in ipairs(Object.indexes) do
        local s = {
            inArg   = T[ overload.i[1] ] or 'ScriptableVar',
            outArg  = T[ overload.o[1] ] or 'ScriptableVar',
            ['in']  = overload.i[1],
            ['out'] = table.concat(overload.o, '|'),
            type    = overload.i[1],
            typeName= typeName,
        }
        local e = Indexator:gsub('\n', '\n'..indent)
        res = res..e:substitute(s)
    end
    return res
end

function G.generateConstructor(typeName, Object)
    local indent = string.rep(' ', 4)
    local counter = 0
    local res = ''
    local function autocounter() counter = counter + 1 return counter end
    for _,overload in ipairs(Object.constructors) do
        assert(type(overload) == 'table')
        local s = {
            -- ['in']  = table.concat(overload, ','),
            inArgs  = G.getArgs(overload, indent..string.rep(' ', 8)),
            num     = autocounter(),
            typeName= typeName,
        }
        local e = CEvaluator:gsub('\n', '\n'..indent)
        res = res..e:substitute(s)
    end
    local s = {
        init       = G.getInits(Object.constructors, indent..string.rep(' ', 4)),
        evaluators = res
    }
    return Constructor:substitute(s)
end

function G.generateCode(Objects)
    local file = [[
// Автоматически сгенерировано в ${time}
// Реализация поддержки скриптабельности объекта в системе USS.
// Ищите по меткам TODO в коде, чтобы выполнить полезную работу.
#ifndef ${guard}
#define ${guard}

#include "USS/Field.h"

//============================= Конструктор ==============================

${constructor}
//================================ Методы ================================

${methods}
//=============================== Свойства ===============================

${properties}
//============================= Индексаторы ==============================

${indexators}

#endif // ${guard}
]]
    local s = {
        time = os.date('UTC+5 %c'),
    }
    for typeName, Object in pairs(Objects) do
        s.guard     = '__'..typeName..'Fields_H__'
        s.methods   = G.generateMethods(typeName, Object)
        s.properties= G.generateProperties(typeName, Object)
        s.indexators= G.generateIndexes(typeName, Object)
        s.constructor=G.generateConstructor(typeName, Object)
        file = file:substitute(s)
        io.output(typeName..'Fields.h')
        io.write(file)
        io.close()
    end
end

return G