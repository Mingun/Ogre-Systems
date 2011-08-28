-- local print_table = require"tools.print_table"
require"lpeg"
-- require"metalua.table2"

lpeg.locale(lpeg)   -- adds locale entries into 'lpeg' table

local keywords = {
'asm',
-- 'auto',
-- 'bitand',
-- 'bitor',
'bool',
'break',
'case',
'catch',
'char',
'class',
-- 'compl',
'const',
'const_cast',
'continue',
'default',
'delete',
'do',
'double',
'dynamic_cast',
'else',
'enum',
'explicit',
'export',
'extern',
'false',
'float',
'for',
'friend',
'goto',
'if',
'inline',
'int',
'long',
'mutable',
'namespace',
'new',
'operator',
'private',
'protected',
'public',
'register',
'reinterpret_cast',
'return',
'short',
'signed',
'sizeof',
'static',
'static_cast',
'struct',
'switch',
'template',
'this',
'throw',
'true',
'try',
'typedef',
'typeid',
'typename',
'union',
'unsigned',
'using',
'virtual',
'void',
'volatile',
'wchar_t',
'while',
}

local space = lpeg.space^0
local any = lpeg.P(1)
-- ключевое_слово[пробелы]
local kw = {}
table.foreach(keywords, function(k, v) kw[v] = lpeg.P(v) * space end)

local function list(elem, delim)
    return (delim ~= nil) and (elem * (delim * elem)^0) or (elem^1)
end
local function s(str)
    return str * space
end
-- Захватывает выражение в скобках
local function balanced(open, close)
    open = lpeg.P(open)
    close = lpeg.P(close)
    return lpeg.P{ open                 -- Открывающее выражение, 
                 * lpeg.C(((1 - (open + close))-- любой символ, кроме границ...
                 + lpeg.V(1))^0)         -- ...или сбалансированное выражение (0 и более),
                 * close }              -- а далее закрывающее выражение
end

local funcBody = balanced('{', '}');
local ctorBody = (s':' * list(any^0, s',') )^-1 * balanced('{', '}');
    
local function constructor(class)
    return class * s'(' * kw.void^-1 * s')' * (s';' + ctorBody)
end
local function destructor(class)
    return s'~' * class * s'(' * kw.void^-1 * s')' * (s';' + funcBody)
end

local function parse(source)
--[[    local grammar = lpeg.P{
        'class';
        openBlock  = '{' * space;
        closeBlock = '}' * space;
        openParenthesis  = '(' * space;
        closeParenthesis = ')' * space;
        delim = ';' * space;
        type = lpeg.alpha^1;
        name = lpeg.alpha^1;
    
        parameter = type * name;
        args = list(parameter, ',');
    }]]
    local type = lpeg.C(lpeg.alpha^1) * space;
    local name = lpeg.C(lpeg.alpha^1) * space;

    local parameter = type * name;
    local args = list(parameter, ',' * space);
    
    local funcDeclare = name * s'(' * (args + kw.void) * s')';
    local staticMethod = kw.static * type * funcDeclare * (';' + funcBody) * space;
    local virtualMethod = kw.virtual^-1 * type * funcDeclare * (';' + funcBody) * space;
    local pureVirtualMethod = kw.virtual * type * funcDeclare * s'=' * s'0' * s';';
    local method = staticMethod + virtualMethod + pureVirtualMethod;

    local access = kw.public + kw.protected + kw.private;
    local classChunk = access * s':' * method^0;
    local inheritance = s':' * list(access * name, s',')
    local classHeader = (kw.class + kw.struct) * name * inheritance^-1 * space;
    
    local class = classHeader * s'{' * (classChunk^0 + method^0) * s'}' * s';';
    -- local class = classHeader * balanced(s'{', any, 0, s'}') * s';';

    -- return class:match(source)
    return lpeg.C(class):match(source)
end
local function example()
    local space = lpeg.space^0
    local name = lpeg.C(lpeg.alpha^1) * space
    local sep = lpeg.S(",;") * space
    local pair = lpeg.Cg(name * "=" * space * name) * sep^-1
    local list = lpeg.Cf(lpeg.Ct("") * pair^0, rawset)
    t = list:match("a=b, c = hi; next = pi")  --> { a = "b", c = "hi", next = "pi" }
    print_table(t)
end

local grammar = lpeg.P{
    'doccomment';
    nl = lpeg.P'\r\n' + lpeg.S'\n\r';
    doccomment = lpeg.V'line' + lpeg.V'block';
    content = lpeg.C((any - '*/')^0);
    line = lpeg.P'//' * lpeg.S'/!' * lpeg.V'content' * lpeg.V'nl';
    block = lpeg.P'/*' * lpeg.S'*!' * lpeg.V'content' * lpeg.P'*/';
}

--[=[print("'"..tostring(parse[[
struct A {
virtual int f(int i, int y);
static Class d(void);
};
]]).."'")]=]
--[=[ print(balanced('{', '}'):match[[
{
{(023)}
virtual int f(int i, int y);
static Class d(void);
{any}
}
]])]=]
