#ifndef __JavaIteratorContainerAdaptors_H_
#define __JavaIteratorContainerAdaptors_H_

#include "Common/JavaIterator.h"

namespace java {

    template<typename Container>
    class STLContainerIterator : public STLIterator< typename Container::iterator
                                                   , typename Container::value_type >
    {
        typedef STLIterator< typename Container::iterator, typename Container::value_type > _Base;
        typedef STLContainerIterator<Container> _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;

        typedef typename _Base::iterator        iterator;
    public:
        explicit STLContainerIterator(Container& c)
            : _Base( c.begin(), c.end() ) {}

        STLContainerIterator(iterator current, iterator end)
            : _Base( current, end ) {}

        virtual const_reference next() { return *this->mCurrent++; }

        virtual Iterator<value_type>* clone() const
        {
            return new _Self( this->current(), this->end() );
        }
    };
    template<typename Container>
    class STLContainerConstIterator : public STLIterator< typename Container::const_iterator
                                                        , typename Container::value_type >
    {
        typedef STLIterator< typename Container::const_iterator, typename Container::value_type > _Base;
        typedef STLContainerConstIterator<Container> _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;

        typedef typename _Base::iterator        iterator;
    public:
        explicit STLContainerConstIterator(const Container& c)
            : _Base( c.begin(), c.end() ) {}

        STLContainerConstIterator(iterator current, iterator end)
            : _Base( current, end ) {}

        virtual const_reference next() { return *this->mCurrent++; }

        virtual Iterator<value_type>* clone() const
        {
            return new _Self( this->current(), this->end() );
        }
    };
    template<typename Container>
    class STLContainerListIterator : public STLListIterator< typename Container::iterator
                                                           , typename Container::value_type >
    {
        typedef STLListIterator< typename Container::iterator, typename Container::value_type > _Base;
        typedef STLContainerListIterator<Container> _Self;
    public:
        typedef typename _Base::value_type      value_type;
        typedef typename _Base::pointer         pointer;
        typedef typename _Base::const_pointer   const_pointer;
        typedef typename _Base::reference       reference;
        typedef typename _Base::const_reference const_reference;

        typedef typename _Base::iterator        iterator;
    protected:
        STLContainerListIterator(iterator current, iterator begin, iterator end)
            : _Base( current, begin, end ) {}
    public:
        STLContainerListIterator(Container& c, bool reverse = false)
            : _Base( c.begin(), c.end(), reverse ) {}

        virtual const_reference prev() { return *(--this->mCurrent); }

        virtual Iterator<value_type>* clone() const
        {
            return new _Self( this->current(), this->begin(), this->end() );
        }
    };

    // Вспомогательные функции

    namespace utils {
        template<class Container>
        STLContainerIterator<Container>* make_stl_container_iterator(Container& c)
        {
            return new STLContainerIterator<Container>(c);
        }
        template<class Container>
        STLContainerIterator<Container>* make_stl_container_iterator(
                const std::pair<typename Container::iterator, typename Container::iterator>& p)
        {
            return new STLContainerIterator<Container>(p.first, p.second);
        }
        template<class Container>
        STLContainerIterator<Container>* make_stl_container_iterator(
                typename Container::iterator& begin, typename Container::iterator& end)
        {
            return new STLContainerIterator<Container>(begin, end);
        }

        // Константные версии

        template<class Container>
        STLContainerConstIterator<Container>* make_stl_container_iterator(const Container& c)
        {
            return new STLContainerConstIterator<Container>(c);
        }
        template<class Container>
        STLContainerConstIterator<Container>* make_stl_container_iterator(
                const std::pair<typename Container::const_iterator, typename Container::const_iterator>& p)
        {
            return new STLContainerConstIterator<Container>(p.first, p.second);
        }
        template<class Container>
        STLContainerConstIterator<Container>* make_stl_container_iterator(
                typename Container::const_iterator& begin, typename Container::const_iterator& end)
        {
            return new STLContainerConstIterator<Container>(begin, end);
        }

    } // namespace utils

} // namespace java

#endif // __JavaIteratorContainerAdaptors_H_
