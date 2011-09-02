#ifndef __USSScriptEngine_H__
#define __USSScriptEngine_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreString.h"
#include "Common/OgreStringVector.h"
#include "ORS/OgreDataStream.h"
#include "USS/ScriptVarList.h"

namespace Script {

    class _ScriptExport ScriptEngine : public ScriptableAlloc
    {
    public:
        enum CompileOption {
            /// Преобразование потока данных не требуется.
            CO_NONE
            /// Компилировать исходный код в байткод.
          , CO_BYTECODE
            /// Компилировать исходный код в машинный код.
          , CO_JIT
        };
        friend class ScriptManager;
    public:
        virtual ~ScriptEngine() {}

        virtual void registerModule(IModulePtr module) = 0;
        virtual void unregisterModule(IModulePtr module) = 0;

        /** Регистрирует указанную фабрику в скриптовом движке.
            После регистрации фабрики скриптабельных объектов, можно будет
            создавать их и управлять ими из скриптов, контролируемых этим
            скриптовым движком.
        @param clazz
            Экземпляр фабрики для регистрации.

        @note
            Для публикации конкретного скриптабельного объекта, который нельзя
            будет создать из скрипта, @link registerObject зарегистрируйте@endlink 
            только его и не регистрируйте его фабрику.
        */
        virtual void registerClass(IClassPtr clazz) = 0;
        virtual void unregisterClass(IClassPtr clazz) = 0;

        /** Регистрирует созданный скриптабельный объект в скриптовом движке.
        @param name
            Имя, под которым объект должен быть известен в скриптах.
            Рекомендуется следовать следующим соглашениям по созданию имен,
            чтобы они могли быть доступны в любом скриптовом движке:
            <ul>
                <li>имя должно состоять из латинских букв ([A-Za-z]) или цифр;</li>
                <li>при этом имя должно начинаться с буквы;</li>
                <li>знаки подчеркивания во многих языках являются допустимым 
                    символом, но если можете их избежать, то лучше их не использовать;</li>
                <li>не используйте общеупотребительные ключевые слова для наименования
                    объектов (например, for, case и т.д.).</li>
            </ul>
            При несоблюдении этих условий скриптовый движок может отказать в 
            регистрации объекта.
        @param o
            Существующий (т.е. созданный (обычно) с помощью своей фабрики) 
            объект #IScriptable.
        @param bAllowDelete
            Опциональный флаг, позволяющий запретить пользователю удаление
            экземпляра этого объекта. По умолчанию пользователь может удалить
            объект из скрипта. Это может пригодится, например, при публикации
            объекта, который предполагает информационную роль и должен существовать
            все время при работе приложения. В этом случае, вероятно, имеет смысл,
            чтобы пользователь вообще не мог сам создавать такие объекты
            (чего можно добиться, просто не регистрируя фабрику для его создания).
        
        @return
            Возвращает @c true, если объект был успешно зарегистрирован и @c false
            иначе (что может, например, произойти, если желаемое имя недопустимо
            для этого скриптового движка).

        @note
            Указатель, передаваемый в эту функцию, не должен быть перемещаемым,
            иначе скриптовый движок может "потерять" ваш объект. Так же на 
            ваших плечах лежит ответственность за то, что время жизни объекта
            будет не меньше, чем время жизни скриптового движка.
        */
        virtual bool registerObject(const String& name, IScriptablePtr o, bool bAllowDelete = true) = 0;
        virtual void unregisterObject(IScriptablePtr o) = 0;

        /** Возвращает список расширений (без точки), с которыми себя ассоциирует 
            этот скриптовый движок. Учтите, что присутствие расширения в этом списке
            вовсе не означает, что файл с таким расширением будет обрабатываться 
            этим движком. Это поведение может быть переопределено @link ScriptManager
            менеджером скриптов@endlink. По умолчанию расширение файла ассоциируется 
            с последним загруженным скриптовым движком, который использует это расширение.
            <p>
            Реализация по умолчанию возвращает пустой список.
        */
        virtual StringVector extensions() const { return StringVector(); }
        
        /// Возвращает версию скриптового движка в виде произвольной строки.
        virtual String version() const = 0;

        /** Возвращает тип скриптового движка. Под этим именем скриптовый движок
            будет зарегистрирован в @link ScriptManager менеджере скриптов@endlink.
        @sa ScriptManager#getScriptEngine
        @sa ScriptManager#exec
        */
        virtual String getType() const = 0;

        /** Выполняет указанную функцию в указанном скрипте. Если имя функции
            не задано, пытается выполнить глобальные инструкции скрипта. Если
            их не найдется, ничего не будет сделано.
        @param script
            Поток данных с выполняемым файлом скрипта.
        @param functionName
            Имя функции, которая должна быть выполнена. Если оно не будет 
            задано, будет сделана попытка выполнить глобальные инструкции в 
            скрипте (т.е. инструкции, не принадлежащие ни одной функции).
        @param args
            Аргументы, передаваемые в функцию. Если функция не будет указана,
            эти аргументы будут доступны скрипту, как глобальные переменные.

        @return
            Список значений, которые вернула функция или глобальный блок скрипта.
        */
        virtual ScriptVarList exec(DataStreamPtr script, const String& functionName, const ScriptVarList& args = ScriptVarList::BLANK) = 0;

        /** Компилирует указанный поток данных.
        @param source
            Поток данных с компилируемым исходником скрипта.
        @param target
            Выходной поток, в который будет записан откомпилированный скрипт.
        @param options
            Параметры компиляции.
        @param stripDebug
            Флаг, указывающий, сохранять ли отладочную информацию. По умолчанию равен @c false.

        @return
            Поток данных со скомпилированным скриптом.
        */
        virtual bool compile(DataStreamPtr source, DataStreamPtr target, CompileOption options, bool stripDebug = false) = 0;

    protected:
        /** Вызывается при дерегистрации скриптового движка. В этой функции необходимо
            освободить все ресурсы, занятые скриптовым движком
        */
        virtual void unregister() {}

        /** Ищет наиболее подходящий по параметру индексатор на основе аргумента, полученного
            по @link Bridge мосту@endlink из движка.
        @param indexators
            Список индексаторов. Если он будет пустым, вернется @c NULL.
        @param bridge
            Мост, с помощью которого отождествляются скриптовые переменные на структуры
            данных конкретного движка.

        @return
            Наиболее подходящий под аргумент индексатор или @c NULL, если такового не оказалось.
        */
        static IIndexator* findIndexator(const IndexatorList& indexators, Bridge& bridge);

        /** Ищет наиболее подходящий по параметрам метод на основе аргументов, полученных
            по @link Bridge мосту@endlink из движка.
        @param indexators
            Список методов. Если он будет пустым, вернется @c NULL.
        @param name
            Имя метода.
        @param bridge
            Мост, с помощью которого отождествляются скриптовые переменные на структуры
            данных конкретного движка.

        @return
            Наиболее подходящая под аргументы перегрузка метода или @c NULL, если таковой не оказалось.

        @notes
            Пример использования:
            @code
            // В одном из методов MyScriptEngine
            IScriptable* o = ...
            IClass* c = o->getClass();
            MyBridge bridge(...);// наследник Bridge

            IMethod* m = findMethod( c->getMethods(), name, bridge );
            if ( m )
            {
                ScriptVarListPtr inArgs = m->inArgs();
                ScriptVarListPtr outArgs = m->outArgs();

                inArgs->unpack( bridge );

                m->eval( o, *inArgs, *outArgs );
                outArgs->pack( bridge );
            } 
            else 
            {
                // обработка ошибки
            }
            @endcode
        */
        static IMethod* findMethod(const MethodList& methods, const String& name, Bridge& bridge);

        static IEvaluator* findEvaluator(const EvaluatorList& evaluators, Bridge& bridge);

        /** Ищет наиболее подходящий по параметрам конструктор на основе аргументов, полученных
            по @link Bridge мосту@endlink из движка.
        @param constructors
            Список конструкторов. Если он будет пустым, вернется @c NULL.
        @param bridge
            Мост, с помощью которого отождествляются скриптовые переменные на структуры
            данных конкретного движка.

        @return
            Наиболее подходящий под аргументы конструктор или @c NULL, если такового не оказалось.
        */
        static IConstructor* findConstructor(const ConstructorList& constructors, Bridge& bridge);
    };

}// namespace Script

#endif // __USSScriptEngine_H__
