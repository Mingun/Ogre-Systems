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

#ifndef __StringConverter_H__
#define __StringConverter_H__

#include "Common/OgrePrerequisites.h"
#include "Common/OgreStringVector.h"
//#include "OgreMath.h"

namespace Ogre {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup General
	*  @{
	*/
	/** Class for converting the core Ogre data types to/from Strings.
    @remarks
        The code for converting values to and from strings is here as a separate
        class to avoid coupling String to other datatypes (and vice-versa) which reduces
        compilation dependency: important given how often the core types are used.
    @par
        This class is mainly used for parsing settings in text files. External applications
        can also use it to interface with classes which use the StringInterface template
        class.
    @par
        The String formats of each of the major types is listed with the methods. The basic types
        like int and Real just use the underlying C runtime library atof and atoi family methods,
        however custom types like Vector3, ColourValue and Matrix4 are also supported by this class
        using custom formats.
    @author
        Steve Streeting
    */
    class _OgreExport StringConverter
    {
    public:

        /** Converts a Real to a String. */
        static String toString(Real val, unsigned short precision = 6, 
            unsigned short width = 0, char fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );

        /** Converts an int to a String. */
        static String toString(int val, unsigned short width = 0, 
            char fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );
#if OGRE_ARCH_TYPE == OGRE_ARCHITECTURE_64 || OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        /** Converts an unsigned int to a String. */
        static String toString(unsigned int val, 
            unsigned short width = 0, char fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );
        /** Converts a size_t to a String. */
        static String toString(size_t val, 
            unsigned short width = 0, char fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );
        #if OGRE_COMPILER == OGRE_COMPILER_MSVC
                /** Converts an unsigned long to a String. */
                static String toString(unsigned long val, 
                    unsigned short width = 0, char fill = ' ', 
                    std::ios::fmtflags flags = std::ios::fmtflags(0) );

        #endif
#else
        /** Converts a size_t to a String. */
        static String toString(size_t val, 
            unsigned short width = 0, char fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );
        /** Converts an unsigned long to a String. */
        static String toString(unsigned long val, 
            unsigned short width = 0, char fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );
#endif
        /** Converts a long to a String. */
        static String toString(long val, 
            unsigned short width = 0, char fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );
        /** Converts a boolean to a String. 
        @param yesNo If set to true, result is 'yes' or 'no' instead of 'true' or 'false'
        */
        static String toString(bool val, bool yesNo = false);

        /** Converts a StringVector to a string.
        @remarks
            Strings must not contain spaces since space is used as a delimiter in
            the output.
        */
        static String toString(const StringVector& val);

        /** Converts a String to a Real. 
        @returns
            0.0 if the value could not be parsed, otherwise the Real version of the String.
        */
        static Real parseReal(const String& val, Real defaultValue = 0);
        
        /** Converts a String to a whole number. 
        @returns
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static int parseInt(const String& val, int defaultValue = 0);
        /** Converts a String to a whole number. 
        @returns
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static unsigned int parseUnsignedInt(const String& val, unsigned int defaultValue = 0);
        /** Converts a String to a whole number. 
        @returns
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static long parseLong(const String& val, long defaultValue = 0);
        /** Converts a String to a whole number. 
        @returns
            0.0 if the value could not be parsed, otherwise the numeric version of the String.
        */
        static unsigned long parseUnsignedLong(const String& val, unsigned long defaultValue = 0);
        /** Converts a String to a boolean. 
        @remarks
            Returns true if case-insensitive match of the start of the string
			matches "true", "yes" or "1", false otherwise.
        */
        static bool parseBool(const String& val, bool defaultValue = 0);
		
        /** Pareses a StringVector from a string.
        @remarks
            Strings must not contain spaces since space is used as a delimiter in
            the output.
        */
        static StringVector parseStringVector(const String& val);
        /** Checks the String is a valid number value. */
        static bool isNumber(const String& val);
    };

	/** @} */
	/** @} */

}



#endif

