#ifndef __LuaFunction_H__
#define __LuaFunction_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "USS/Function.h"

extern "C" {
struct lua_State;
}
using namespace Script;

/** Класс для поддержки анонимных функций в качестве обработчиков событий.
*/
class LuaFunction : public Function
{
    typedef Function _Base;
private:
    lua_State* L;
    int mIndex;
public:
    LuaFunction(lua_State* l, int index);
    virtual ~LuaFunction();
    virtual void eval(const ScriptVarList& inArgs, ScriptVarList& outArgs) const;
};

#endif // __LuaFunction_H__
