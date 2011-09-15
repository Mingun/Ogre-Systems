#ifndef __JavaIteratorMapAdaptors_H_
#define __JavaIteratorMapAdaptors_H_

#include "Common/JavaIterator.h"

namespace java {

    template<typename Map, typename Iter>
    class STLMapKeyIterator : public STLIterator<Iter, typename Map::key_type>
    {
        typedef STLIterator<Iter, typename Map::key_type> _Base;
        typedef STLMapKeyIterator<Map, Iter>    _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;

        typedef typename _Base::iterator        iterator;
    public:
        STLMapKeyIterator(iterator current, iterator end)
            : _Base( current, end ) {}

        virtual const_reference next() { return (*this->mCurrent++).first; }

        virtual Iterator<value_type>* clone() const
        {
            return new _Self( this->current(), this->end() );
        }
    };
    template<typename Map>
    class STLKeySetIterator : public STLMapKeyIterator<Map, typename Map::iterator>
    {
    public:
        explicit STLKeySetIterator(Map &m)
            : _Base( m.begin(), m.end() ) {}

        STLKeySetIterator(iterator current, iterator end)
            : _Base( current, end ) {}
        
        virtual Iterator<value_type>* clone() const
        {
            return new _Self( this->current(), this->end() );
        }
    };
    template<typename Map>
    class STLKeySetConstIterator : public STLMapKeyIterator<Map, typename Map::const_iterator>
    {
    public:
        explicit STLKeySetConstIterator(const Map &m)
            : _Base( m.begin(), m.end() ) {}

        STLKeySetConstIterator(iterator current, iterator end)
            : _Base( current, end ) {}

        virtual Iterator<value_type>* clone() const
        {
            return new _Self( this->current(), this->end() );
        }
    };
    
    /// Итератор по значениям отображения
    template<typename Map, typename Iter>
    class STLMapValueIterator : public STLIterator<Iter, typename Map::mapped_type>
    {
        typedef STLIterator<Iter, typename Map::mapped_type> _Base;
        typedef STLMapValueIterator<Map, Iter>  _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;

        typedef typename _Base::iterator        iterator;
    public:
        STLMapValueIterator(iterator current, iterator end)
            : _Base( current, end ) {}

        virtual const_reference next() { return (*this->mCurrent++).second; }

        virtual Iterator<value_type>* clone() const
        {
            return new _Self( this->current(), this->end() );
        }
    };
    template<typename Map>
    class STLValueSetIterator : public STLMapValueIterator<Map, typename Map::iterator>
    {
        typedef STLMapValueIterator<Map, typename Map::iterator> _Base;
        typedef STLValueSetIterator<Map>  _Self;
    public:
        explicit STLValueSetIterator(Map &m)
            : _Base( m.begin(), m.end() ) {}

        STLValueSetIterator(iterator current, iterator end)
            : _Base( current, end ) {}
        
        virtual Iterator<value_type>* clone() const
        {
            return new _Self( this->current(), this->end() );
        }
    };

    template<typename Map>
    class STLValueSetConstIterator : public STLMapValueIterator<Map, typename Map::const_iterator>
    {
        typedef STLMapValueIterator<Map, typename Map::const_iterator> _Base;
        typedef STLValueSetConstIterator<Map>  _Self;
    public:
        explicit STLValueSetConstIterator(const Map &m)
            : _Base( m.begin(), m.end() ) {}

        STLValueSetConstIterator(iterator current, iterator end)
            : _Base( current, end ) {}
        
        virtual Iterator<value_type>* clone() const
        {
            return new _Self( this->current(), this->end() );
        }
    };
    
    /// Итератор по одинаковым значениям из multimap
    template<typename Map, typename Iter>
    class STLMapSameKeyIterator : public STLIterator<Iter, typename Map::mapped_type>
    {
        typedef STLIterator<Iter, typename Map::mapped_type> _Base;
        typedef STLMapSameKeyIterator<Map, Iter> _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;

        typedef typename _Base::iterator        iterator;
    public:
        explicit STLMapSameKeyIterator(iterator current)
            : _Base( current, current ) {}

        virtual bool hasNext() const 
        {   // Если достигли конца, выходим
            if ( !_Base::hasNext() )
                return false;
            iterator _next = this->mCurrent;
            return (*this->mCurrent).first == (*++_next).first; 
        }
        virtual const_reference next() { return (*this->mCurrent++).second; }

        virtual Iterator<value_type>* clone() const
        {
            return new _Self(this->current());
        }
    };
    template<typename Map>
    class STLSameKeyValueSetIterator : public STLMapSameKeyIterator<Map, typename Map::iterator>
    {
        typedef STLMapSameKeyIterator<Map, typename Map::iterator> _Base;
        typedef STLSameKeyValueSetIterator<Map> _Self;
    public:
        explicit STLSameKeyValueSetIterator(iterator current)
            : _Base( current ) {}
        virtual Iterator<value_type>* clone() const
        {
            return new _Self(this->current());
        }
    };
    /// Итератор по одинаковым значениям из multimap
    template<typename Map>
    class STLSameKeyValueSetConstIterator : public STLMapSameKeyIterator<Map, typename Map::const_iterator>
    {
        typedef STLMapSameKeyIterator<Map, typename Map::const_iterator> _Base;
        typedef STLSameKeyValueSetConstIterator<Map> _Self;
    public:
        explicit STLSameKeyValueSetConstIterator(iterator current)
            : _Base( current ) {}
        virtual Iterator<value_type>* clone() const
        {
            return new _Self(this->current());
        }
    };
    
    // Вспомогательные функции

    namespace utils {
        template<class Map>
        STLKeySetIterator<Map>* make_keyset_iterator(Map& m)
        {
            return new STLKeySetIterator<Map>(m);
        }
        template<class Map>
        STLKeySetIterator<Map>* make_keyset_iterator(
                const std::pair<typename Map::iterator, typename Map::iterator>& p)
        {
            return new STLKeySetIterator<Map>(p.first, p.second);
        }
        template<class Map>
        STLKeySetIterator<Map>* make_keyset_iterator(
                typename Map::iterator& begin, typename Map::iterator& end)
        {
            return new STLKeySetIterator<Map>(begin, end);
        }

        template<class Map>
        STLKeySetConstIterator<Map>* make_keyset_iterator(const Map& m)
        {
            return new STLKeySetConstIterator<Map>(m);
        }
        template<class Map>
        STLKeySetConstIterator<Map>* make_keyset_iterator(
                const std::pair<typename Map::const_iterator, typename Map::const_iterator>& p)
        {
            return new STLKeySetConstIterator<Map>(p.first, p.second);
        }
        template<class Map>
        STLKeySetConstIterator<Map>* make_keyset_iterator(
                typename Map::const_iterator& begin, typename Map::const_iterator& end)
        {
            return new STLKeySetConstIterator<Map>(begin, end);
        }

    } // namespace utils

} // namespace java

#endif // __JavaIteratorMapAdaptors_H_
