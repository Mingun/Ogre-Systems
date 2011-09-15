-- Шаблоны
local T = {__doc__ = 'Шаблоны кода'}

-- Шаблон для замены
T.patt = '%${(%w+)}'

-- Шаблон метода, не имеющего перегрузок (экономичнее по абстракциям,
-- чуточку быстрее работает)
T.SimpleMethod = 
[[// Метод ${name}(${in}) -> ${out}
class Method_${name} : public SimpleMethod
{
    typedef SimpleMethod _Base;
public:
    Method_${name}() : _Base() {}
public:// Evaluator
    virtual std::auto_ptr<ScriptVarList> inArgs() const {
        std::auto_ptr<ScriptVarList> args(new ScriptVarList());
        ${inArgs}
        return args;
    }
    virtual std::auto_ptr<ScriptVarList> outArgs() const {
        std::auto_ptr<ScriptVarList> args(new ScriptVarList());
        ${outArgs}
        return args;
    }
    virtual void eval(Scriptable* obj, const ScriptVarList& inArgs, ScriptVarList& outArgs) const {
        ${typeName}* s = static_cast<${typeName}*>(obj);
        //TODO Сделайте полезную работу здесь
        
    }
public:// Scriptable
    virtual String getName() const { return "${name}"; }
};
]]

-- Шаблон метода, имеющего перегрузки
T.Method = 
[[// Перегруженный метод ${name}()
class Method_${name} : public Method
{
    typedef Method _Base;
private:
    ${evaluators}
public:
    Method_${name}() 
        : _Base() 
    {
        ${init}
    }
public:// Scriptable
    virtual String getName() const { return "${name}"; }
};
]]

-- Шаблон конструктора
T.Constructor = 
[[// Конструктор объекта
class Constructor : public Method
{
    typedef Method _Base;
private:
    ${evaluators}
public:// Scriptable
    Constructor() : _Base() {
        ${init}
    }
    // Конструктор имеет специальное имя
    virtual String getName() const { return "~"; }
};
]]

-- Свойство
T.Property = 
[[// Свойство ${name} -> ${out}
class Property_${name} : public Property
{
    typedef Property _Base;
public:
    Property_${name}() : _Base() {}
public:// Evaluator
    virtual std::auto_ptr<ScriptVarList> inArgs() const {
        return outArgs();
    }
    virtual std::auto_ptr<ScriptVarList> outArgs() const {
        std::auto_ptr<ScriptVarList> args(new ScriptVarList());
        ${outArg}
        return args;
    }
    virtual void eval(Scriptable* obj, const ScriptVarList& inArgs, ScriptVarList& outArgs) const {
        ${typeName}* s = static_cast<${typeName}*>(obj);
        // получение свойства
        if ( inArgs.empty() ) {
            static_cast<${type}*>(outArgs[0])->value() = s->${fnGet}();
        } else {
            s->${fnSet}( static_cast<${type}*>(inArgs[0])->value() );
        }
    }
public:// Scriptable
    virtual String getName() const { return "${name}"; }
};
]]

-- Шаблон вычислителя
T.Evaluator = 
[[// ${name}(${in}) -> ${out}
class E${num} : public Evaluator
{
public:// Evaluator
    virtual std::auto_ptr<ScriptVarList> inArgs() const {
        std::auto_ptr<ScriptVarList> args(new ScriptVarList());
        ${inArgs}
        return args;
    }
    virtual std::auto_ptr<ScriptVarList> outArgs() const {
        std::auto_ptr<ScriptVarList> args(new ScriptVarList());
        ${outArgs}
        return args;
    }
    virtual void eval(Scriptable* obj, const ScriptVarList& inArgs, ScriptVarList& outArgs) const {
        ${typeName}* s = static_cast<${typeName}*>(obj);
        //TODO Сделайте полезную работу здесь
        
    }
};
]]

-- Шаблон вычислителя конструктора
T.CEvaluator = 
[[
class E${num} : public CBase {
public:
    virtual std::auto_ptr<ScriptVarList> inArgs() const {
        std::auto_ptr<ScriptVarList> args(new ScriptVarList());
        ${inArgs}
        return args;
    }
};
]]

-- Шаблон индексатора объекта
T.Indexator = 
[[// Индексатор объекта obj[${in}] -> ${out}
class ${type}Indexator : public Indexator 
{
public:
    virtual std::auto_ptr<ScriptVar> inArg() const {
        return std::auto_ptr<ScriptVar>(new ${inArg}());
    }
    virtual std::auto_ptr<ScriptVar> outArg() const {
        return std::auto_ptr<ScriptVar>(new ${outArg}());
    }
    virtual void eval(Scriptable* obj, const ScriptVar& inArg, ScriptVar& outArg) const {
        ${typeName}* s = static_cast<${typeName}*>(obj);
        const ${inArg}& in = static_cast<const ${inArg}&>(inArg);
        ${outArg}& out = static_cast<${outArg}&>(inArg);
        //TODO Сделайте полезную работу здесь
        
    }
};
]]

-- Инициализатор вычислителей
T.Init = 
[[mCallers.push_back( new ${evaluator}() );
]]

-- Заполнитель списка параметров
T.Args = 
[[args->add(new ${type}("${name}"));
]]

return T