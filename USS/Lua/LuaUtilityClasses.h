
class Options 
{
public:
    LuaScriptEngine* mEngine;
    DataStreamPtr    mSource;
    DataStreamPtr    mTarget;
    bool mStripDebug;
public:
    Options(LuaScriptEngine* e, DataStreamPtr s, DataStreamPtr t, bool d)
        : mEngine(e), mSource(s), mTarget(t), mStripDebug(d) {}
};

class IFieldFinder : public IField
{
    String mKey;
public:
    IFieldFinder(const String& key) : mKey(key) {}
    virtual const String& doc() const { return StringUtil::BLANK; }
    virtual const String& getName() const { return mKey; }
    virtual TYPE getType() const { return Property; }
};

/** Упаковывает переменную на вершину стека Lua.
*/
class Packer : public LuaBridge, public std::unary_function<const ScriptVar*, void>
{
public:
    Packer(lua_State *L) 
        : LuaBridge(L) {}
    void operator()(const ScriptVar* var)
    {
        TRACE_STACK;
        var->pack(*this);
    }
};
/** Распаковывает переменную со стека Lua с указанной позиции.
*/
class Unpacker : public LuaBridge, public std::unary_function<ScriptVar*, void>
{
public:
    Unpacker(lua_State *l, int index = 0) 
        : LuaBridge(l, index) {}
    void operator()(ScriptVar* var)
    {
        var->unpack(*this);
    }
};
/** Распаковывает значения из таблицы, как если бы они были
    переданы как обычные параметры. Позволяет вызывать функции
    объектов в декларативном стиле, указывая имена параметров.
    Особенно удобно таким образом создавать объекты.
*/
class TableUnpacker : public LuaBridge, public std::unary_function<ScriptVar*, void>
{
    int mTableIndex;
public:
    TableUnpacker(lua_State *L, int index = 0) 
        : LuaBridge(L, -1), mTableIndex(index) {}
    void operator()(ScriptVar* var)
    {
        lua_getfield( L, mTableIndex, var->name().c_str() );

        if ( !lua_isnil(L, -1) )
        {
            var->unpack(*this);
        }
        lua_pop(L, 1);
    }
};

class TableChecker : public LuaBridge
{
    mutable int mEstimate;
    int mTableIndex;
public:
    TableChecker(lua_State *L, int index = 0) 
        : LuaBridge(L, -1), mEstimate(0), mTableIndex(index) {}
    void operator()(const ScriptVar* var)
    {
        // вычисляем оценку
        // Надбавка за аргументы по умолчанию позволит корректно обработать
        // ситуацию, когда вызывается функция вида
        //   fn(arg = default)
        // в виде
        //   fn()
        if ( var->hasDefault() )
            ++mEstimate;
        lua_getfield( L, mIndex, var->name().c_str() );

        if ( var->check( *this ) )
            ++mEstimate;
        lua_pop(L, 1);
    }
    inline int estimate() const { return mEstimate; }
};