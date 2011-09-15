#ifndef __JavaIteratorCombinatorAdaptors_H_
#define __JavaIteratorCombinatorAdaptors_H_

#include "Common/JavaIterator.h"

namespace java {

    /** Итератор, комбинирующий @c Count итераторов.
    */
    template<unsigned int Count, typename E>
    class ComplexIterator : public Iterator<E>
    {
        typedef Iterator<E>                     _Base;
        typedef ComplexIterator<Count, E>       _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;

        typedef Iterator<E>* IteratorPtr;
    protected:
        Iterator<E>*                its[Count];
        mutable unsigned int        mCurrent;
    public:
        explicit ComplexIterator(const Iterator<E>* begin[Count])
            : mCurrent(0)
        {
            for ( unsigned int i = 0; i < Count; ++i )
            {// автоматическое копирование в шаблоне указателя
                its[i] = begin[i]->clone();
            }
        }
        ComplexIterator(const Iterator<E>* it1, ...)
            : mCurrent(0)
        {
            its[0] = it1->clone();
            va_list args;
            va_start(args, it1);
            for ( unsigned int i = 1; i < Count-1; ++i )
            {// автоматическое копирование в шаблоне указателя
                its[i] = va_arg( args, const Iterator<E>* )->clone();
            }
            va_end(args);
        }

        virtual bool hasNext() const
        {
            if ( its[mCurrent]->hasNext() )
                return true;
            if ( mCurrent < Count )
                return its[++mCurrent]->hasNext();
            return false;
        }
        virtual const_reference next()
        {
            return its[mCurrent]->next();
        }
        virtual Iterator<E>* clone() const
        {
            const Iterator<E>* its2[Count];
            for ( unsigned int i = 0; i < Count; ++i )
                its2[i] = its[i];
            // автоматическое копирование в шаблоне указателя
            return new _Self( its2 );
        }
    };

    // специализации

    // Итератор вообще не хранит данных, так как все заведомо известно
    template<typename E>
    class ComplexIterator<0, E> : public Iterator<E>
    {
        typedef Iterator<E>                     _Base;
        typedef ComplexIterator<0, E>           _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;
    public:
        explicit ComplexIterator(const Iterator<E>**) {}
        ComplexIterator(const Iterator<E>*, ...) {}

        virtual bool hasNext() const { return false; }
        virtual E const& next()      { assert(!"Your must NOT call next() if hasNext() return false!"); }
        virtual Iterator<E>* clone() const { return new _Self(NULL); }
    };

    template<typename E>
    class ComplexIterator<1, E> : public Iterator<E>
    {
        typedef Iterator<E>                     _Base;
        typedef ComplexIterator<1, E>           _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;
    protected:
        java::Iterator<E>* pImpl;
    public:
        explicit ComplexIterator(const Iterator<E>** iter)
            : pImpl( *iter ){}
        ComplexIterator(const Iterator<E>* iter, ...)
            : pImpl( iter ) {}

        virtual bool hasNext() const { return pImpl->hasNext(); }
        virtual const_reference next() { return pImpl->next(); }
        virtual Iterator<E>* clone() const { return new _Self( &pImpl ); }
    };

    /// Псевдо-итератор по одному элементу.
    template<typename E>
    class OneIterator : public Iterator<E>
    {
        typedef Iterator<E>     _Base;
        typedef OneIterator<E>  _Self;
    private:
        value_type mValue;
        bool mExtracted;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;
    public:
        explicit OneIterator(const_reference value) 
            : mValue( value ), mExtracted( false ) {}
        OneIterator(const_reference value, bool extracted) 
            : mValue( value ), mExtracted( extracted ) {}
        virtual bool hasNext() const { return !mExtracted; }
        virtual const_reference next() {
            mExtracted = true;
            return mValue;
        }

        virtual Iterator<E>* clone() const {
            return new _Self(mValue, mExtracted);
        }
    };

} // namespace java

#endif // __JavaIteratorCombinatorAdaptors_H_
