/*-------------------------------------------------------------------------
This source file is a part of OGRE
(Object-oriented Graphics Rendering Engine)

For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE
-------------------------------------------------------------------------*/
#ifndef __OgrePrerequisites_H__
#define __OgrePrerequisites_H__

// Platform-specific stuff
#include "Common/OgrePlatform.h"

// Needed for OGRE_WCHAR_T_STRINGS below
#include <string>


// configure memory tracking
#if OGRE_DEBUG_MODE
#	if OGRE_MEMORY_TRACKER_DEBUG_MODE
#		define OGRE_MEMORY_TRACKER 1
#	else
#		define OGRE_MEMORY_TRACKER 0
#	endif
#else
#	if OGRE_MEMORY_TRACKER_RELEASE_MODE
#		define OGRE_MEMORY_TRACKER 1
#	else
#		define OGRE_MEMORY_TRACKER 0
#	endif
#endif




namespace Ogre {
    // Define ogre version
    #define OGRE_VERSION_MAJOR 1
    #define OGRE_VERSION_MINOR 7
    #define OGRE_VERSION_PATCH 1
	#define OGRE_VERSION_SUFFIX ""
    #define OGRE_VERSION_NAME "Cthugha"

    #define OGRE_VERSION    ((OGRE_VERSION_MAJOR << 16) | (OGRE_VERSION_MINOR << 8) | OGRE_VERSION_PATCH)

    // define the real number values to be used
    // default to use 'float' unless precompiler option set
    #if OGRE_DOUBLE_PRECISION == 1
		/** Software floating point type.
		@note Not valid as a pointer to GPU buffers / parameters
		*/
        typedef double Real;
    #else
		/** Software floating point type.
		@note Not valid as a pointer to GPU buffers / parameters
		*/
        typedef float Real;
    #endif

    #if OGRE_COMPILER == OGRE_COMPILER_GNUC && OGRE_COMP_VER >= 310 && !defined(STLPORT)
	#   if OGRE_COMP_VER >= 430
	#       define HashMap ::std::tr1::unordered_map
	#		define HashSet ::std::tr1::unordered_set
	#    else
	#       define HashMap ::__gnu_cxx::hash_map
	#       define HashSet ::__gnu_cxx::hash_set
	#    endif
    #else
    #   if OGRE_COMPILER == OGRE_COMPILER_MSVC
    #       if OGRE_COMP_VER >= 1600 // VC++ 10.0
	#			define HashMap ::std::tr1::unordered_map
	#           define HashSet ::std::tr1::unordered_set
	#		elif OGRE_COMP_VER > 1300 && !defined(_STLP_MSVC)
    #           define HashMap ::stdext::hash_map
	#           define HashSet ::stdext::hash_set
    #       else
    #           define HashMap ::std::hash_map
	#           define HashSet ::std::hash_set
    #       endif
    #   else
    #       define HashMap ::std::hash_map
	#       define HashSet ::std::hash_set
    #   endif
    #endif

    /** In order to avoid finger-aches :)
    */
    typedef unsigned char uchar;
    typedef unsigned short ushort;
    typedef unsigned int uint;
	typedef unsigned long ulong;


// Useful threading defines
#include "Threading/OgreThreadDefines.h"

// Pre-declare classes
// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files
    class Archive;
    class ArchiveFactory;
    class ArchiveManager;
    class AutoParamDataSource;
    class Codec;
	class DataStream;
	class DefaultWorkQueue;
    class DynLib;
    class DynLibManager;
    class ErrorDialog;
    class Factory;
    class Log;
    class LogManager;
	class ManualResourceLoader;
	class ManualObject;
    class MemoryManager;
	class Plugin;
    class Profile;
	class Profiler;
    class Resource;
	class ResourceBackgroundQueue;
	class ResourceGroupManager;
    class ResourceManager;
	class ScriptCompiler;
	class ScriptCompilerManager;
	class ScriptLoader;
    class Serializer;
	class StreamSerialiser;
    class StringConverter;
    class StringInterface;
	class WorkQueue;
}

/* Include all the standard header *after* all the configuration
settings have been made.
*/
#include "Common/OgreStdHeaders.h"
#include "OMS/OgreMemoryAllocatorConfig.h"


namespace Ogre
{
#if OGRE_STRING_USE_CUSTOM_MEMORY_ALLOCATOR
	#if OGRE_WCHAR_T_STRINGS
		typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, STLAllocator<wchar_t,GeneralAllocPolicy > >	_StringBase;
	#else
		typedef std::basic_string<char, std::char_traits<char>, STLAllocator<char,GeneralAllocPolicy > >	_StringBase;
	#endif

	#if OGRE_WCHAR_T_STRINGS
		typedef std::basic_stringstream<wchar_t,std::char_traits<wchar_t>,STLAllocator<wchar_t,GeneralAllocPolicy >> _StringStreamBase;
	#else
		typedef std::basic_stringstream<char,std::char_traits<char>,STLAllocator<char,GeneralAllocPolicy > > _StringStreamBase;
	#endif

	#define StdStringT(T) std::basic_string<T, std::char_traits<T>, std::allocator<T> >
	#define CustomMemoryStringT(T) std::basic_string<T, std::char_traits<T>, STLAllocator<T,GeneralAllocPolicy> >

	template<typename T>
	bool operator <(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<0;
	}
	template<typename T>
	bool operator <(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<0;
	}
	template<typename T>
	bool operator <=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<=0;
	}
	template<typename T>
	bool operator <=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<=0;
	}
	template<typename T>
	bool operator >(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>0;
	}
	template<typename T>
	bool operator >(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>0;
	}
	template<typename T>
	bool operator >=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>=0;
	}
	template<typename T>
	bool operator >=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>=0;
	}

	template<typename T>
	bool operator ==(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())==0;
	}
	template<typename T>
	bool operator ==(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())==0;
	}

	template<typename T>
	bool operator !=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())!=0;
	}
	template<typename T>
	bool operator !=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())!=0;
	}

	template<typename T>
	CustomMemoryStringT(T) operator +=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l)+=o.c_str();
	}
	template<typename T>
	CustomMemoryStringT(T) operator +=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l.c_str())+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l)+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l.c_str())+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +(const T* l,const CustomMemoryStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l)+=o;
	}

	#undef StdStringT
	#undef CustomMemoryStringT

#else
	#if OGRE_WCHAR_T_STRINGS
		typedef std::wstring _StringBase;
	#else
		typedef std::string _StringBase;
	#endif

	#if OGRE_WCHAR_T_STRINGS
		typedef std::basic_stringstream<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > _StringStreamBase;
	#else
		typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > _StringStreamBase;
	#endif

#endif

	typedef _StringBase String;
	typedef _StringStreamBase StringStream;
	typedef StringStream stringstream;

}

//for stl containter
namespace Ogre
{
	template <typename T, typename A = STLAllocator<T, GeneralAllocPolicy> >
	struct deque
	{
#if OGRE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	    typedef typename std::deque<T, A> type;
#else
		typedef typename std::deque<T> type;
#endif
	};

	template <typename T, typename A = STLAllocator<T, GeneralAllocPolicy> >
	struct vector
	{
#if OGRE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::vector<T, A> type;
#else
		typedef typename std::vector<T> type;
#endif
	};

	template <typename T, typename A = STLAllocator<T, GeneralAllocPolicy> >
	struct list
	{
#if OGRE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	    typedef typename std::list<T, A> type;
#else
		typedef typename std::list<T> type;
#endif
	};

	template <typename T, typename P = std::less<T>, typename A = STLAllocator<T, GeneralAllocPolicy> >
	struct set
	{
#if OGRE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	    typedef typename std::set<T, P, A> type;
#else
		typedef typename std::set<T, P> type;
#endif
	};

	template <typename K, typename V, typename P = std::less<K>, typename A = STLAllocator<std::pair<const K, V>, GeneralAllocPolicy> >
	struct map
	{
#if OGRE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	    typedef typename std::map<K, V, P, A> type;
#else
		typedef typename std::map<K, V, P> type;
#endif
	};

	template <typename K, typename V, typename P = std::less<K>, typename A = STLAllocator<std::pair<const K, V>, GeneralAllocPolicy> >
	struct multimap
	{
#if OGRE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::multimap<K, V, P, A> type;
#else
		typedef typename std::multimap<K, V, P> type;
#endif
	};

	template <typename T, typename P = std::less<T>, typename A = STLAllocator<T, GeneralAllocPolicy> >
	struct multiset
	{
#if OGRE_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::multiset<T, P, A> type;
#else
		typedef typename std::multiset<T, P> type;
#endif
	};
} // Ogre

#endif // __OgrePrerequisites_H__


