
#include "Common/OgreString.h"

#include "USS/ScriptEngine.h"
#include "USS/Bridge.h"
#include "USS/IIndexator.h"
#include "USS/IMethod.h"
#include "USS/IConstructor.h"
#include "USS/IField.h"
#include "USS/ScriptVar.h"

namespace Script {

    class IMethodFinder : public IMethod
    {
        String mKey;
    public:
        IMethodFinder(const String& key) : mKey(key) {}
        virtual const String& doc() const { return StringUtil::BLANK; }
        virtual const String& getName() const { return mKey; }
        
        virtual ScriptVarListPtr inArgs()  const { return ScriptVarListPtr(); }
        virtual ScriptVarListPtr outArgs() const { return ScriptVarListPtr(); }
        virtual void eval(IObject* object, const ScriptVarList& inArgs, ScriptVarList& outArgs) const {}
    };

    class Checker : public std::unary_function<const ScriptVar*, void>
    {
        mutable int mEstimate;
        Bridge* mBridge;
    public:
        Checker(Bridge* bridge)
            : mBridge(bridge), mEstimate(0) {}
        /// Возвращает @c true, если указанная переменная соответствует
        /// текущему параметру, полученному из движка.
        inline void operator()(const ScriptVar* var)
        {
            mEstimate += (omitted(var) || var->check(*mBridge));
            mBridge->next();
        }
        /// Возвращает @c true, если параметр мог быть опущен.
        /// Параметр опущен, если фактического параметра нет, а он имеет значение по умолчанию.
        inline bool omitted(const ScriptVar* var) const
        { return mBridge->isEmpty() && var->hasDefault(); }
        inline int estimate() const { return mEstimate; }
        inline void cleanup() { delete mBridge; }
    };

    /*template<typename It>
    It findEvaluator(const It& begin, const It& end)
    {
        typedef std::pair<int, It> Result;
        for ( It it = begin; it != end; ++it )
        {
            IEvaluator* e = *it;
            ScriptVarListPtr inArgs = e->inArgs();

            // Вычислитель без параметров самый низкоприоритетный, так как оценку
            // по нему вычислять невозможно - не с чем сравнивать.
            if ( !inArgs->empty() )
            {
                Checker ch = std::for_each( inArgs->begin()
                                          , inArgs->end()
                                          , Checker(bridge.clone()) );
                if ( ch.estimate() > max.first )
                    max = std::make_pair( ch.estimate(), it );
                ch.cleanup();
            } else if ( bridge.isEmpty() )
                return it;
        }
        return max.second;
    }*/

    IIndexator* ScriptEngine::findIndexator(const IndexatorList& indexators, Bridge& bridge)
    {
        IndexatorList::const_iterator it = indexators.begin();
        for ( ; it != indexators.end(); ++it )
        {
            if ( (*it)->keyArg()->check( bridge ) )
            {
                return *it;
            }
        }
        return NULL;
    }

    IMethod* ScriptEngine::findMethod(const MethodList& methods, const String& name, Bridge& bridge)
    {
        typedef std::pair<int, IMethod*> Result;

        Result max(0, NULL);
        MethodList::const_iterator it = methods.find( &IMethodFinder(name) );
        
        IMethod* m = *it;
        // проверяем все поля с указанным именем
        while ( m->getName() == name )
        {
            ScriptVarListPtr inArgs = m->inArgs();

            // Метод без параметров самый низкоприоритетный, так как оценку
            // по нему вычислять невозможно - не с чем сравнивать.
            if ( !inArgs->empty() )
            {
                Checker ch = std::for_each( inArgs->begin()
                                          , inArgs->end()
                                          , Checker(bridge.clone()) );
                if ( ch.estimate() > max.first )
                    max = std::make_pair( ch.estimate(), m );
                ch.cleanup();
            } else if ( bridge.isEmpty() )
                return m;
            ++it;
            m = *it;
        }
        return max.second;
    }

    IEvaluator* ScriptEngine::findEvaluator(const EvaluatorList& evaluators, Bridge& bridge)
    {
        typedef std::pair<int, IEvaluator*> Result;

        Result max(0, NULL);
        EvaluatorList::const_iterator it = evaluators.begin();
        
        for ( ; it != evaluators.end(); ++it )
        {
            IEvaluator* e = *it;
            ScriptVarListPtr inArgs = e->inArgs();

            // Вычислитель без параметров самый низкоприоритетный, так как оценку
            // по нему вычислять невозможно - не с чем сравнивать.
            if ( !inArgs->empty() )
            {
                Checker ch = std::for_each( inArgs->begin()
                                          , inArgs->end()
                                          , Checker(bridge.clone()) );
                if ( ch.estimate() > max.first )
                    max = std::make_pair( ch.estimate(), e );
                ch.cleanup();
            } else if ( bridge.isEmpty() )
                return e;
        }
        return max.second;
    }

    IConstructor* ScriptEngine::findConstructor(const ConstructorList& constructors, Bridge& bridge)
    {
        typedef std::pair<int, IConstructor*> Result;
        
        Result max(0, NULL);
        ConstructorList::const_iterator it = constructors.begin();
        for ( ; it != constructors.end(); ++it )
        {
            IConstructor* c = *it;
            ScriptVarListPtr inArgs = c->inArgs();

            // Конструктор без параметров самый низкоприоритетный, так как оценку
            // по нему вычислять невозможно - не с чем сравнивать.
            if ( inArgs->empty() )
            {
                if ( bridge.isEmpty() )
                    return c;
                continue;
            }

            Checker ch = std::for_each( inArgs->begin()
                                      , inArgs->end()
                                      , Checker(bridge.clone()) );
            if ( ch.estimate() > max.first )
                max = std::make_pair( ch.estimate(), c );
            ch.cleanup();
        }
        return max.second;
    }
}// namespace Script