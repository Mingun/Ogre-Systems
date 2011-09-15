cmake_minimum_required(VERSION 2.6)

set(PROJECT Core)

set(HEADERS OgreConfig.h
            ${OGRE_SYSTEMS}/OLS/OgreLog.h
            ${OGRE_SYSTEMS}/OLS/OgreLogManager.h
            
            ${OGRE_SYSTEMS}/OMS/OgreMemoryNedAlloc.h
            ${OGRE_SYSTEMS}/OMS/OgreMemoryNedPooling.h
            
            ${OGRE_SYSTEMS}/ODLS/OgreDynLib.h
            ${OGRE_SYSTEMS}/ODLS/OgreDynLibManager.h
            
            ${OGRE_SYSTEMS}/OES/OgreException.h
            
            ${OGRE_SYSTEMS}/OPS/OgrePlugin.h
            ${OGRE_SYSTEMS}/OPS/PluginManager.h
            
            ${OGRE_SYSTEMS}/ORS/OgreDataStream.h
            
            ${OGRE_SYSTEMS}/Common/JavaIterator.h
            ${OGRE_SYSTEMS}/Common/OgreFactoryObj.h
            ${OGRE_SYSTEMS}/Common/OgreIteratorWrapper.h
            ${OGRE_SYSTEMS}/Common/OgrePrerequisites.h
            ${OGRE_SYSTEMS}/Common/OgreSingleton.h
            ${OGRE_SYSTEMS}/Common/OgreString.h
            ${OGRE_SYSTEMS}/Common/OgreStringConverter.h
            ${OGRE_SYSTEMS}/Common/OgreStringVector.h
            
            ${OGRE_SYSTEMS}/OAS/OgreArchive.h
            ${OGRE_SYSTEMS}/OAS/OgreArchiveFactory.h
            ${OGRE_SYSTEMS}/OAS/OgreArchiveManager.h
            ${OGRE_SYSTEMS}/OAS/OgreFileSystem.h
)
set(SOURCES ${OGRE_SYSTEMS}/OLS/OgreLog.cpp
            ${OGRE_SYSTEMS}/OLS/OgreLogManager.cpp
            
            ${OGRE_SYSTEMS}/OMS/OgreMemoryNedAlloc.cpp
            ${OGRE_SYSTEMS}/OMS/OgreMemoryNedPooling.cpp
            
            ${OGRE_SYSTEMS}/ODLS/OgreDynLib.cpp
            ${OGRE_SYSTEMS}/ODLS/OgreDynLibManager.cpp
            
            ${OGRE_SYSTEMS}/OES/OgreException.cpp
            
            ${OGRE_SYSTEMS}/OPS/PluginManager.cpp
            
            ${OGRE_SYSTEMS}/ORS/OgreDataStream.cpp
            
            ${OGRE_SYSTEMS}/Common/OgreString.cpp
            ${OGRE_SYSTEMS}/Common/OgreStringConverter.cpp
            
            ${OGRE_SYSTEMS}/OAS/OgreArchiveManager.cpp
            ${OGRE_SYSTEMS}/OAS/OgreFileSystem.cpp
)

project(${PROJECT})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${OGRE_SYSTEMS})

add_definitions(-D_USRDLL)

# Add some compiler flags
if(MSVC)
    add_definitions(/W4)
    # Use relative paths
    # This is mostly to reduce path size for command-line limits on windows
    # This seems to break Xcode projects so definitely don't enable on Apple builds
    set(CMAKE_USE_RELATIVE_PATHS true)
    set(CMAKE_SUPPRESS_REGENERATION true)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall -pedantic)
else()
    message("Unknown compiler")
endif()

source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

# Find Boost
# Prefer static linking in all cases
if (WIN32 OR APPLE)
    set(Boost_USE_STATIC_LIBS TRUE)
else ()
    # Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
    set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
endif ()
if (APPLE)
    if(OGRE_BUILD_PLATFORM_APPLE_IOS)
        set(Boost_COMPILER "-xgcc42")
    endif()
endif()
set(Boost_ADDITIONAL_VERSIONS "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
# Components that need linking (NB does not include header-only components like bind)
set(CORE_BOOST_COMPONENTS thread date_time)
find_package(Boost COMPONENTS ${CORE_BOOST_COMPONENTS})# QUIET)
if (NOT Boost_FOUND)
    # Try again with the other type of libs
    if(Boost_USE_STATIC_LIBS)
        set(Boost_USE_STATIC_LIBS)
    else()
        set(Boost_USE_STATIC_LIBS ON)
    endif()
    find_package(Boost COMPONENTS ${CORE_BOOST_COMPONENTS})# QUIET)
endif()
find_package(Boost)# QUIET)

include_directories(${Boost_INCLUDE_DIRS})
add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${Boost_LIBRARIES})