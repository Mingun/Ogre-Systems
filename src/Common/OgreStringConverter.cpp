/*
-----------------------------------------------------------------------------
This source file is part of OGRE
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
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "Common/OgreStringConverter.h"

namespace Ogre {

    //-----------------------------------------------------------------------
    String StringConverter::toString(Real val, unsigned short precision, 
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
		static char buffer[128] = "";
		int n = snprintf(buffer, 128, "%f", val);
		return String(buffer, n);
#else
        stringstream stream;
        stream.precision(precision);
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
#endif
    }
    //-----------------------------------------------------------------------
    String StringConverter::toString(int val, 
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        stringstream stream;
		stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
#if OGRE_ARCH_TYPE == OGRE_ARCHITECTURE_64 || OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    String StringConverter::toString(unsigned int val, 
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        stringstream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String StringConverter::toString(size_t val, 
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        stringstream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
    //-----------------------------------------------------------------------
    String StringConverter::toString(unsigned long val, 
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        stringstream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }

#endif
    //-----------------------------------------------------------------------
#else
    String StringConverter::toString(size_t val, 
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        stringstream stream;
		stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String StringConverter::toString(unsigned long val, 
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        stringstream stream;
		stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
#endif
    String StringConverter::toString(long val, 
        unsigned short width, char fill, std::ios::fmtflags flags)
    {
        stringstream stream;
		stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << val;
        return stream.str();
    }
    //-----------------------------------------------------------------------
    String StringConverter::toString(bool val, bool yesNo)
    {
        if (val)
        {
            if (yesNo)
            {
                return "yes";
            }
            else
            {
                return "true";
            }
        }
        else
            if (yesNo)
            {
                return "no";
            }
            else
            {
                return "false";
            }
    }
    //-----------------------------------------------------------------------
    String StringConverter::toString(const StringVector& val)
    {
		stringstream stream;
        StringVector::const_iterator i, iend, ibegin;
        ibegin = val.begin();
        iend = val.end();
        for (i = ibegin; i != iend; ++i)
        {
            if (i != ibegin)
                stream << " ";

            stream << *i; 
        }
        return stream.str();
    }
    //-----------------------------------------------------------------------
    Real StringConverter::parseReal(const String& val, Real defaultValue)
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
		Real ret = 0;
		int n = sscanf(val.c_str(), "%f", &ret);
		
		if(n == 0){
			// Nothing read, so try integer format
			int ret2 = 0;
			n = sscanf(val.c_str(), "%d", &ret2);
			if(n == 1)
				ret = (Real)ret2;
		}
#else
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		Real ret = defaultValue;
		str >> ret;
#endif
        return ret;
    }
    //-----------------------------------------------------------------------
    int StringConverter::parseInt(const String& val, int defaultValue)
    {
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		int ret = defaultValue;
		str >> ret;

        return ret;
    }
    //-----------------------------------------------------------------------
    unsigned int StringConverter::parseUnsignedInt(const String& val, unsigned int defaultValue)
    {
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		unsigned int ret = defaultValue;
		str >> ret;

		return ret;
    }
    //-----------------------------------------------------------------------
    long StringConverter::parseLong(const String& val, long defaultValue)
    {
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		long ret = defaultValue;
		str >> ret;

		return ret;
    }
    //-----------------------------------------------------------------------
    unsigned long StringConverter::parseUnsignedLong(const String& val, unsigned long defaultValue)
    {
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		unsigned long ret = defaultValue;
		str >> ret;

		return ret;
    }
    //-----------------------------------------------------------------------
    bool StringConverter::parseBool(const String& val, bool defaultValue)
    {
		if ((StringUtil::startsWith(val, "true") || StringUtil::startsWith(val, "yes")
			|| StringUtil::startsWith(val, "1")))
			return true;
		else if ((StringUtil::startsWith(val, "false") || StringUtil::startsWith(val, "no")
			|| StringUtil::startsWith(val, "0")))
			return false;
		else
			return defaultValue;
    }
    //-----------------------------------------------------------------------
    StringVector StringConverter::parseStringVector(const String& val)
    {
        return StringUtil::split(val);
    }
	//-----------------------------------------------------------------------
	bool StringConverter::isNumber(const String& val)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
		float test;
		int n = sscanf(val.c_str(), "%f", &test);
		return n == 1;
#else
		StringStream str(val);
		float tst;
		str >> tst;
		return !str.fail() && str.eof();
#endif
	}
}


