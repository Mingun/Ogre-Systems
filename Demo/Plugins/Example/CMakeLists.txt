cmake_minimum_required(VERSION 2.6)

set(PROJECT Example)
set(CORE ../../Core)

if(NOT OGRE_SYSTEMS)
    set(OGRE_SYSTEMS "../../Ogre Systems")
endif()
set(HEADERS OgreConfig.h
            Plugin.h
)
set(SOURCES Plugin.cpp
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

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT} ${CORE})