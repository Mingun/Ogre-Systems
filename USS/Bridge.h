#ifndef __USSBridge_H__
#define __USSBridge_H__

#pragma once

namespace Script {

    class Bridge
    {
    public:
        virtual ~Bridge() {}
        // Группа методов для передачи абстрактных аргументов
        // в структуры скриптового движка.
        virtual void pack(bool value) = 0;
        virtual void pack(int value) = 0;
        virtual void pack(double value) = 0;
        virtual void pack(char value) = 0;
        virtual void pack(const String& value) = 0;
        virtual void pack(const Iterator* value) = 0;
        virtual void pack(const IScriptablePtr& value) = 0;
        virtual void pack(const Hash& value) = 0;
        virtual void pack(const Function* value) = 0;
        virtual void pack(const void* value) = 0;

        // Группа методов для передачи агрументов из внутренних структур
        // скриптового движка в абстрактные аргументы.
        virtual void unpack(bool& value) = 0;
        virtual void unpack(int& value) = 0;
        virtual void unpack(double& value) = 0;
        virtual void unpack(char& value) = 0;
        virtual void unpack(String& value) = 0;
        virtual void unpack(Iterator*& value) = 0;
        virtual void unpack(IScriptablePtr& value) = 0;
        virtual void unpack(Hash& value) = 0;
        virtual void unpack(Function*& value) = 0;
        virtual void unpack(void*& value) = 0;

        // Группа методов для проверки соответствия ожидаемых аргументов
        // фактическим аргументам, содержащихся во внутренних структурах
        // скриптового движка.
        virtual int check(bool value) = 0;
        virtual int check(int value) = 0;
        virtual int check(double value) = 0;
        virtual int check(char value) = 0;
        virtual int check(const String& value) = 0;
        virtual int check(const Iterator* value) = 0;
        virtual int check(const IScriptablePtr& value) = 0;
        virtual int check(const Hash& value) = 0;
        virtual int check(const Function* value) = 0;
        virtual int check(const void* value) = 0;
        // Начальная проверка. Если она провалена, аргументы из списка не проверяются.
        virtual int check(const ScriptVarList& vars) = 0;

        virtual void next() = 0;
        /// Возвращает @c true, если скриптовый движок не имеет переменной для текущего положения.
        virtual bool isEmpty() const = 0;

        virtual Bridge* clone() const = 0;
    };

}// namespace Script

#endif // __USSBridge_H__