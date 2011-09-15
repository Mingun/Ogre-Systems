#ifndef JAVAITERATOR_H
#define JAVAITERATOR_H

#include <memory>
#include <stdarg.h>
#include <assert.h>
#include <ostream>

namespace java {

    /**
    @param E
        Тип итерируемых значений.
    */
    template<typename E>
    class Iterator
    {
        typedef Iterator<E> _Self;
    public:
        typedef E           value_type;
        typedef E*          pointer;
        typedef E const*    const_pointer;
        typedef E&          reference;
        typedef E const&    const_reference;
    public:
        virtual ~Iterator() {}
        virtual bool hasNext() const = 0;
        virtual const_reference next() = 0;

        virtual Iterator<E>* clone() const = 0;
    };
    template<>
    class Iterator<void>
    {
        typedef Iterator<void> _Self;
    public:
        typedef void        value_type;
        typedef void*       pointer;
        typedef void const* const_pointer;
        typedef void        reference;
        typedef void        const_reference;
    public:
        virtual ~Iterator() {}
        virtual bool hasNext() const = 0;
        virtual void next() = 0;
        
        virtual Iterator<void>* clone() const = 0;
    };

    template<typename E>
    class ListIterator : public Iterator<E>
    {
        typedef Iterator<E>                     _Base;
        typedef ListIterator<E>                 _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;
    public:
        virtual ~ListIterator() {}
        virtual bool hasPrev() const = 0;
        virtual const_reference prev() = 0;
    };

    /** Адаптер STL-подобного итератора в Java-подобный итератор.
    @param Iter
        Тип оборачиваемого STL-подобного итератора.
    @param E
        Тип возвращаемого итератором значения.
    */
    template<typename Iter, typename E>
    class STLIterator : public Iterator<E>
    {
        typedef Iterator<E>                     _Base;
        typedef STLIterator<Iter, E>            _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;

        typedef Iter                            iterator;
    protected:
        Iter mCurrent;
        Iter mEnd;
    public:
        STLIterator(Iter begin, Iter end)
            : mCurrent( begin ), mEnd( end ) {}

        explicit STLIterator(std::pair<Iter, Iter>& p)
            : mCurrent( p.first ), mEnd( p.second ) {}

        template<class Container>
        explicit STLIterator(Container& c)
            : mCurrent( c.begin() ), mEnd( c.end() ) {}

        virtual ~STLIterator() {}

        virtual bool hasNext() const { return mCurrent != mEnd; }

        Iter current() const { return mCurrent; }
        Iter end() const { return mEnd; }

//            size_t size() const { return mEnd - mBegin; }
//            size_t index() const { return mCurrent - mBegin; }
    };

    template<typename Iter, typename E>
    class STLListIterator : public STLIterator<Iter, E>
                          , public ListIterator<E>
    {
        typedef STLIterator<Iter, E>            _Base;
        typedef STLListIterator<Iter, E>        _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;

        typedef typename _Base::iterator        iterator;
    protected:
        iterator mBegin;
    public:
        STLListIterator(iterator begin, iterator end, bool reverse = false)
            : _Base( reverse ? end : begin, end ), mBegin( begin ) {}

        STLListIterator(const std::pair<Iter, Iter>& p, bool reverse = false)
            : _Base( reverse ? p.second : p.first, p.second ), mBegin( p.first ) {}

        STLListIterator(iterator current, iterator begin, iterator end)
            : _Base( current, end ), mBegin( begin ) {}

        STLListIterator(iterator current, const std::pair<iterator, iterator>& p)
            : _Base( current, p.second ), mBegin( p.first ) {}

        template<class Container>
        STLListIterator(const Container& c, bool reverse = false)
            : _Base( reverse ? c.end() : c.begin(), c.end() ), mBegin( c.begin() ) {}

        virtual ~STLListIterator() {}

        //virtual bool hasNext() const   { return _Base::hasNext();  }
        //virtual const_reference next() { return _Base::next(); }

        virtual bool hasPrev() const { return this->mCurrent != this->mBegin; }

        Iter begin() const { return mBegin; }

        void setToBegin() { this->mCurrent = this->mBegin; }
        void setToEnd()   { this->mCurrent = this->mEnd;   }

//            size_t size()  const { return this->mEnd     - this->mBegin; }
//            size_t index() const { return this->mCurrent - this->mBegin; }
    };

    template<typename E>
    std::ostream& operator <<(std::ostream& os, const Iterator<E>& it)
    {
        std::auto_ptr<Iterator<E> > i( it.clone() );
        if ( i->hasNext() )
            os << i->next();
        while ( i->hasNext() )
            os << ", " << i->next();
        return os;
    }

} // namespace java

#endif // JAVAITERATOR_H
