
/* convert a stack index to positive */
#define abs_index(L, i)		((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : \
					lua_gettop(L) + (i) + 1)
// Алгоритм:
// 1. Найти поле с указанным именем. Если поле не найдено, перейти к шагу 4.
// 2. Если поле является свойством, то вычислить его значение и выйти.
// 3. Вернуть функцию-замыкание с информацией о том, метод с каким именем был вызван.
// 4. Если объект индексируеый, то проиндексировать его. В случае успеха выйти.
// 5. Перенаправить индексацию в метатаблицу (разрешено использование метаметодов).
int LuaScriptEngine::Scriptable::__index(lua_State *L)
{
    // стек: [IScriptable key]
    IScriptablePtr o = getScriptable(L, 1);
    const IClass* c = o->getClass();

    FieldList fields = c->getFields();
    FieldList::const_iterator it;

    // 1. Найти поле с указанным именем.
    // если ключ является строковым, то он может быть именем поля
    if ( lua_type(L, 2) == LUA_TSTRING )
    {
        String key = lua_tostring(L, 2);

        it = fields.find( &IFieldFinder(key) );
        if ( it != fields.end() )
        {// объект имеет поле с указанным именем
            const IField* f = *it;
            // 2. Если поле является свойством, то вычислить его значение и выйти.
            if ( f->getType() == IField::Property )
            {
                const IProperty* p = static_cast<const IProperty*>(f);
                ScriptVarPtr prop = p->propArg();
                p->get( o.get(), prop );
                prop->pack( LuaBridge(L) );
                return 1;
            }
            // 3. Вернуть функцию-замыкание с информацией о том, метод с каким именем был вызван.
            // Класс и имя вызываемого метода
            lua_pushlightuserdata(L, const_cast<IClass*>(c)); // стек: [cls]
            lua_pushvalue(L, 2);                              // стек: [cls key]
            lua_pushcclosure(L, LuaScriptEngine::accessor, 2);// стек: [fn]
            return 1;
        }
    }

    // 4. Если объект индексируеый, то проиндексировать его. В случае успеха выйти.
    // ищем наиболее подходящий индексатор
    IIndexator* i = findIndexator( c->getIndexators(), LuaBridge(L, 2) );
    if ( i )
    {// объект не имеет поля с указанным именем, но индексируемый
        LuaBridge bridge(L, 2);
        ScriptVarPtr keyArg = i->keyArg();
        ScriptVarPtr valueArg = i->valueArg();

        keyArg->unpack( bridge );
        i->get( o.get(), keyArg, valueArg );

        valueArg->pack( bridge );
        return 1;
    }

    // 5. Перенаправить индексацию в метатаблицу (разрешено использование метаметодов).
    if ( !lua_getmetatable(L, 1) )
        return 0;
                         // стек: [IScriptable key mt]
    lua_pushvalue(L, 2); // стек: [IScriptable key mt key]
    lua_gettable(L, -2); // стек: [IScriptable key mt[key]]
    return 1;
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Присваивание объекту по индексу некоего значения
// Алгоритм:
// 1. Ищем свойство с указанным именем. Если оно найдено, устанавливаем его и выходим.
// 2. Ищем подходящий индексатор. Если найден, присваиваем по индексу и выходим.
// 3. Перенаправляем индексацию в метатаблицу.
int LuaScriptEngine::Scriptable::__newindex(lua_State *L)
{
    // стек: [IScriptable key value]
    IScriptablePtr o = getScriptable(L, 1);
    const IClass* c = o->getClass();

    // 1. Ищем свойство с указанным именем.
    // если ключ является строковым, то он может быть именем поля
    if ( lua_type(L, 2) == LUA_TSTRING )
    {
        String key = lua_tostring(L, 2);
        IProperty* p = c->getProperty(key);
        // Если оно найдено, устанавливаем его и выходим.
        if ( p )
        {
            LuaBridge bridge(L, 3);
            ScriptVarPtr propArg = p->propArg();

            luaL_argcheck(L, propArg->check(bridge), 3, 
                lua_pushfstring(L, "property '%s' require %s, got %s"
                               , p->getName().c_str()
                               , propArg->typeName().c_str()
                               , lua_typename(L, lua_type(L, 3))));

            propArg->unpack( bridge );
            p->set( o.get(), propArg );
            return 0;
        }
    }

    // 2. Ищем подходящий индексатор. (начиная с позиции 2 в стеке)
    IIndexator* i = findIndexator( o->getClass()->getIndexators(), LuaBridge(L, 2) );
    // Если найден, присваиваем по индексу и выходим.
    if ( i )
    {
        LuaBridge bridge(L);
        ScriptVarPtr keyArg = i->keyArg();
        ScriptVarPtr valueArg = i->valueArg();

        keyArg->unpack( bridge );
        valueArg->unpack( bridge );
        i->set( o.get(), keyArg, valueArg );

        return 0;
    }

    // 3. Перенаправляем индексацию в метатаблицу.
    lua_getmetatable(L, 1); // стек: [IScriptable key value mt]
    lua_pushvalue(L, 2);    // стек: [IScriptable key value mt key]
    lua_pushvalue(L, 3);    // стек: [IScriptable key value mt key value]
    lua_settable(L, -3);    // стек: [IScriptable key value mt]
    lua_pop(L, 1);          // стек: [IScriptable key value]
//TODO или так?
    //lua_insert(L, 2);       // стек: [IScriptable mt key value]
    //lua_settable(L, -3);    // стек: [IScriptable mt]
    return 0;
}
//------------------------------------------------------------------------
// Алгоритм:
// 1. Если объект вызываемый, вызвать его и выйти.
int LuaScriptEngine::Scriptable::__call(lua_State *L)
{
    // стек: [IScriptable ...]
    IScriptablePtr o = getScriptable(L, 1);
    const IClass* c = o->getClass();

    EvaluatorList evaluators = c->getEvaluators();
    IEvaluator* e = findEvaluator( evaluators, LuaBridge(L, 2) );

    ScriptVarListPtr inArgs;
    if ( !e )
    {
        e = findEvaluatorTable( L, evaluators );
        if ( !e )
        {
            return 0;
        }
        inArgs = e->inArgs();
        std::for_each( inArgs->begin(), inArgs->end(), TableUnpacker(L, 2) );
    } else {
        inArgs = e->inArgs();
        inArgs->unpack( LuaBridge(L, 2) );
    }

    ScriptVarListPtr outArgs = e->outArgs();
    e->eval( o.get(), *inArgs, *outArgs );
    outArgs->pack( LuaBridge(L) );
    return outArgs->size();
}
//------------------------------------------------------------------------
int LuaScriptEngine::Scriptable::__tostring(lua_State *L)
{
    IScriptablePtr o = getScriptable(L, 1);
    if ( o.isNull() )
        return 0;// Не наш объект оставим в покое

    String str = o->toString();
    lua_pushlstring( L, str.c_str(), str.size() );
    return 1;
}

#undef abs_index